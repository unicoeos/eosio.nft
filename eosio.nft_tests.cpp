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
      abi_ser.set_abi(abi);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(eosio.nft);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

   fc::variant get_stats( const string& symbolname )
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(eosio.nft), symbol_code, N(stat), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "stats", data );
   }

   fc::variant get_account( account_name acc, const string& symbolname)
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(eosio.nft), acc, N(accounts), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data );
   }

   fc::variant get_token(account_name acc, id_type token_id) 
   {
      vector<char> data = get_row_by_account( N(eosio.nft), acc, N(token), token_id );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "token", data );
   }

   action_result create( account_name issuer,
                string symbol ) {

      return push_action( N(eosio.nft), N(create), mvo()
           ( "issuer", issuer)
           ( "symbol", symbol)
      );
   }

   action_result issue( account_name issuer, account_name to, asset quantity, vector<string> uris, string memo ) {
      return push_action( issuer, N(issue), mvo()
           ( "to", to)
           ( "quantity", quantity)
	   ( "uris", uris)
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

   action_result burn( account_name owner, id_type token_id, string sym ){
   	return push_action( owner, N(burn), mvo()
	   ( "owner", owner)
	   ( "token_id", token_id)
	   ( "sym", sym)
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


BOOST_FIXTURE_TEST_CASE( issue_tests, nft_tester ) try {

   auto newtoken = create( N(alice), string("TKN"));
   produce_blocks(1);

   vector<string> uris = {"uri"};

   issue( N(alice), N(alice), asset::from_string("1 TKN"), uris, "hola" );

   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1 TKN")
      ("issuer", "alice")
   );

   /*auto tokenval = get_token(N(alice), 1);
   REQUIRE_MATCHING_OBJECT( tokenval, mvo()
      ("id", 1)
      ("uri", "uri")
      ("owner", "alice")
      ("value", "1 TKN")
   );*/


   auto alice_balance = get_account(N(alice), "0,TKN");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "1 TKN")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "quantity must be a whole number" ),
      issue( N(alice), N(alice), asset::from_string("1.05 TKN"), uris, "hola" )
   );

   string memo;
   for(auto i=0; i<100; i++)
   {
	memo += "hola";
   }

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "memo has more than 256 bytes" ),
      issue( N(alice), N(alice), asset::from_string("1 TKN"), uris, memo )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol does not exist, create token before issue" ),
      issue( N(alice), N(alice), asset::from_string("1 TTT"), uris, "hole" )
   );


   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive quantity of NFTs" ),
      issue( N(alice), N(alice), asset::from_string("-1 TKN"), uris, "hole" )
   );


   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "mismatch between number of tokens and uris provided" ),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, "hole" ) 
   );

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(alice), N(alice), asset::from_string("1 TKN"), uris, "hola" )
   );


   uris.push_back("uri2");

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(alice), N(alice), asset::from_string("2 TKN"), uris, "hola" )
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_tests, nft_tester ) try {

   /*auto token = create( N(alice), asset::from_string("1000 CERO"));
   produce_blocks(1);

   issue( N(alice), N(alice), asset::from_string("1000 CERO"), "hola" );

   auto stats = get_stats("0,CERO");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1000 CERO")
      ("max_supply", "1000 CERO")
      ("issuer", "alice")
   );

   auto alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "1000 CERO")
   );

   transfer( N(alice), N(bob), asset::from_string("300 CERO"), "hola" );

   alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "700 CERO")
      ("frozen", 0)
      ("whitelist", 1)
   );

   auto bob_balance = get_account(N(bob), "0,CERO");
   REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
      ("balance", "300 CERO")
      ("frozen", 0)
      ("whitelist", 1)
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "overdrawn balance" ),
      transfer( N(alice), N(bob), asset::from_string("701 CERO"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must transfer positive quantity" ),
      transfer( N(alice), N(bob), asset::from_string("-1000 CERO"), "hola" )
   );
   */

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( burn_tests, nft_tester ) try {

	auto token = create( N(alice), string("NFT"));
    	
	produce_blocks(1);
	
        vector<string> uris = {"uri1", "uri2"};

	issue( N(alice), N(alice), asset::from_string("2 NFT"), uris, "issue 2 tokens" );

	burn( N(alice), 1, string("NFT"));

	auto stats = get_stats("0,NFT");
	REQUIRE_MATCHING_OBJECT( stats, mvo()
          ("supply", "1 NFT")
	  ("issuer", "alice")
	);				       

        auto alice_balance = get_account(N(alice), "0,NFT");
	REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
          ("balance", "1 NFT")
	);

	BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token not owned by account" ),
		burn( N(bob), 0, string("NFT") )
	);


} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
