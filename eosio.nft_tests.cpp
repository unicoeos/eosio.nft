#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <eosio.nft/eosio.nft.wast.hpp>
#include <eosio.nft/eosio.nft.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

typedef uint64_t id_type;
typedef string uri_type;

class nft_tester : public tester {
public:

   nft_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(carol), N(eosio.nft) } );
      produce_blocks( 2 );

      set_code( N(eosio.nft), eosio_nft_wast );
      set_abi( N(eosio.nft), eosio_nft_abi );

      produce_blocks();

      const auto& accnt = control->db().get<account_object,by_name>( N(eosio.nft) );
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(eosio.nft);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data, abi_serializer_max_time );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

					 
   fc::variant get_stats( const string& symbolname )
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(eosio.nft), symbol_code, N(stat), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "stats", data, abi_serializer_max_time );
   }

   fc::variant get_account( account_name acc, const string& symbolname)
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(eosio.nft), acc, N(accounts), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data, abi_serializer_max_time );
   }

   fc::variant get_token(id_type token_id) 
   {
      vector<char> data = get_row_by_account( N(eosio.nft), N(eosio.nft), N(token), token_id );
      FC_ASSERT(!data.empty(), "empty token");
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "token", data, abi_serializer_max_time );
   }

   action_result create( account_name issuer,
                string symbol ) {

      return push_action( N(eosio.nft), N(create), mvo()
           ( "issuer", issuer)
           ( "symbol", symbol)
      );
   }

   action_result issue( account_name issuer, account_name to, asset quantity, vector<string> uris, string name, string memo ) {
      return push_action( issuer, N(issue), mvo()
           ( "to", to)
           ( "quantity", quantity)
	   ( "uris", uris)
	   ( "name", name)
           ( "memo", memo)
      );
   }

   action_result transfer( account_name from,
                  account_name to,
                  id_type      id,
                  string       memo ) {
      return push_action( from, N(transfer), mvo()
           ( "from", from)
           ( "to", to)
           ( "id", id)
           ( "memo", memo)
      );
   }

   action_result burn( account_name owner, id_type token_id ){
   	return push_action( owner, N(burn), mvo()
	   ( "owner", owner)
	   ( "token_id", token_id)
	);
   }

   abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(eosio_nft_tests)

BOOST_FIXTURE_TEST_CASE( create_tests, nft_tester ) try {

   auto token = create( N(alice), string("NFT"));
   auto stats = get_stats("0,NFT");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 NFT") 
      ("issuer", "alice")
   );
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "issuer account does not exist" ),
	create( N(dummy), string("TKN"))
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( symbol_already_exists, nft_tester ) try {

   auto token = create( N(alice), string("NFT"));
   auto stats = get_stats("0,NFT");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 NFT")
      ("issuer", "alice")
   );
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol already exists" ),
                        create( N(alice), string("NFT"))
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE ( issue_multi_tests, nft_tester ) try {

	auto newtoken = create( N(alice), string("TKN"));
   	produce_blocks(2);

      	vector<string> uris = {"uri1", "uri2", "uri3", "uri4", "uri5"};

        issue( N(alice), N(alice), asset::from_string("5 TKN"), uris, "nft1", "hola" );

	auto stats = get_stats("0,TKN");
	REQUIRE_MATCHING_OBJECT( stats, mvo()
		("supply", "5 TKN")
		("issuer", "alice")
	);

	for(auto i=0; i<5; i++)
	{
		//string indx = to_string(i);
		auto tokenval = get_token((id_type)i);
		REQUIRE_MATCHING_OBJECT( tokenval, mvo()
			("id", i)
			("uri", uris[i])
			("owner", "alice")
			("value", "1 TKN")
			("name", "nft1")
		);
	}


	auto alice_balance = get_account(N(alice), "0,TKN");
	REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
		("balance", "5 TKN")
	);

} FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE( issue_tests, nft_tester ) try {

   auto newtoken = create( N(alice), string("TKN"));
   produce_blocks(1);

   vector<string> uris = {"uri"};

   issue( N(alice), N(bob), asset::from_string("1 TKN"), uris, "nft1", "hola" );

   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1 TKN")
      ("issuer", "alice")
   );

   auto tokenval = get_token(0);
   REQUIRE_MATCHING_OBJECT( tokenval, mvo()
      ("id", "0")
      ("uri", "uri")
      ("owner", "bob")
      ("value", "1 TKN")
      ("name", "nft1")
   );


   auto bob_balance = get_account(N(bob), "0,TKN");
   REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
      ("balance", "1 TKN")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "to account does not exist" ),
      issue( N(alice), N(dummy), asset::from_string("1 TKN"), uris, "nft1", "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "quantity must be a whole number" ),
      issue( N(alice), N(alice), asset::from_string("1.05 TKN"), uris, "nft1", "hola" )
   );

   string memo;
   for(auto i=0; i<100; i++)
   {
	memo += "hola";
   }

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "memo has more than 256 bytes" ),
      issue( N(alice), N(alice), asset::from_string("1 TKN"), uris, "nft1", memo )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol does not exist, create token before issue" ),
      issue( N(alice), N(alice), asset::from_string("1 TTT"), uris, "nft1", "hole" )
   );


   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive quantity of NFTs" ),
      issue( N(alice), N(alice), asset::from_string("-1 TKN"), uris, "nft1", "hole" )
   );


   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "mismatch between number of tokens and uris provided" ),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, "nft1", "hole" ) 
   );

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(alice), N(alice), asset::from_string("1 TKN"), uris, "nft1", "hola" )
   );


   uris.push_back("uri2");

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, "nft1", "hola" )
   );

   string name;
   for(auto i=0; i<10; i++)
   {
       name += "nft1";
   }

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "name has more than 32 bytes" ),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, name, "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "name is empty" ),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, "", "hola" )
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_tests, nft_tester ) try {

   auto token = create( N(alice), string("NFT"));
   produce_blocks(1);

   vector<string> uris = {"uri", "uri2", "uri3"};

   issue( N(alice), N(alice), asset::from_string("3 NFT"), uris, "nft1", "hola" );

   transfer( N(alice), N(bob), 0, "send token 0 to bob" );


   transfer( N(alice), N(bob), 1, "send token 1 to bob" );


   auto alice_balance = get_account(N(alice), "0,NFT");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "1 NFT")
   );

   auto bob_balance = get_account(N(bob), "0,NFT");
   REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
      ("balance", "2 NFT")
   );

   auto tokenval = get_token(1);
   REQUIRE_MATCHING_OBJECT( tokenval, mvo()
        ("id", "1")
        ("uri", "uri2")
	("owner", "bob")
	("value", "1 NFT")
	("name", "nft1")
   );
   

   transfer (N(bob), N(carol), 1, "send token 1 to carol");

   auto bob_balance1 = get_account(N(bob), "0,NFT");
   REQUIRE_MATCHING_OBJECT( bob_balance1, mvo()
        ("balance", "1 NFT")		   
   );
   
   auto carol_balance = get_account(N(carol), "0,NFT");
   REQUIRE_MATCHING_OBJECT( carol_balance, mvo()
        ("balance", "1 NFT")
   );
   
   auto tokenval_1 = get_token(1);
   REQUIRE_MATCHING_OBJECT( tokenval_1, mvo()
      	("id", "1")
      	("uri", "uri2")
	("owner", "carol")
	("value", "1 NFT")
	("name", "nft1")
   );
   
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "sender does not own token with specified ID" ),
      transfer( N(alice), N(carol), 0, "send from non-owner" )
   );	

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "cannot transfer to self" ),
      transfer( N(alice), N(alice), 1, "send to the issuer" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "to account does not exist" ),
      transfer( N(alice), N(dummy), 1, "send to non-existing" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with specified ID does not exist" ),
      transfer( N(alice), N(bob), 3, "no token id" )
   );

   string memo;
   for(auto i=0; i<100; i++)
   {
      memo += "longmemo";
   }

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "memo has more than 256 bytes" ),
      transfer( N(alice), N(bob), 1, memo )
   );


} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( burn_tests, nft_tester ) try {

	auto token = create( N(alice), string("NFT"));
    	
	produce_blocks(1);
	
        vector<string> uris = {"uri1", "uri2"};

	issue( N(alice), N(alice), asset::from_string("2 NFT"), uris, "nft1", "issue 2 tokens" );

	burn( N(alice), 1);

	auto stats = get_stats("0,NFT");
	REQUIRE_MATCHING_OBJECT( stats, mvo()
          ("supply", "1 NFT")
	  ("issuer", "alice")
	);				       

        auto alice_balance = get_account(N(alice), "0,NFT");
	REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
          ("balance", "1 NFT")
	);
       
	BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with id does not exist" ),
	   burn( N(alice), 100200 )
	);

        BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token not owned by account" ),
	   burn( N(bob), 0 )
	);

	burn( N(alice), 0);
	auto stats2 = get_stats("0,NFT");
	REQUIRE_MATCHING_OBJECT( stats2, mvo()
		("supply", "0 NFT")
		("issuer", "alice")
	);
	

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
