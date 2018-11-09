#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;
typedef uint128_t uuid;
typedef uint64_t id_type;
typedef string uri_type;

CONTRACT nft : public eosio::contract {

     public:
	using contract::contract;
        nft( name receiver, name code, datastream<const char*> ds)
		: contract(receiver, code, ds), tokens(receiver, receiver.value) {}


        ACTION create(name issuer, std::string symbol);

        ACTION issue(name to,
                   asset quantity,
                   vector<string> uris,
		   string name,
                   string memo);

        ACTION transferid(name from,
                      name to,
                      id_type id,
                      string memo);

	ACTION transfer(name from,
                      name to,
                      asset quantity,
                      string memo);

        ACTION burn(name owner,
                  id_type token_id);

	ACTION setrampayer(name payer, id_type id);


        TABLE account {

            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };


        TABLE stats {
            asset supply;
            name issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
            uint64_t get_issuer() const { return issuer.value; }
        };


        TABLE token {
            id_type id;          // Unique 64 bit identifier,
            uri_type uri;        // RFC 3986
            name owner;  	 // token owner
            asset value;         // token value (1 SYS)
	    string tokenName;	 // token name

            id_type primary_key() const { return id; }
            uint64_t get_owner() const { return owner.value; }
            string get_uri() const { return uri; }
            asset get_value() const { return value; }
	    uint64_t get_symbol() const { return value.symbol.code().raw(); }
	    string get_name() const { return tokenName; }

	    // generated token global uuid based on token id and
	    // contract name, passed in the argument
	    uuid get_global_id(name self) const
	    {
		uint128_t self_128 = static_cast<uint128_t>(self.value);
		uint128_t id_128 = static_cast<uint128_t>(id);
		uint128_t res = (self_128 << 64) | (id_128);
		return res;
	    }

	    string get_unique_name() const
	    {
		string unique_name = tokenName + "#" + std::to_string(id);
		return unique_name;
	    }
        };

	using account_index = eosio::multi_index<"accounts"_n, account>;

	using currency_index = eosio::multi_index<"stat"_n, stats,
	                       indexed_by< "byissuer"_n, const_mem_fun< stats, uint64_t, &stats::get_issuer> > >;

	using token_index = eosio::multi_index<"token"_n, token,
	                    indexed_by< "byowner"_n, const_mem_fun< token, uint64_t, &token::get_owner> >,
			    indexed_by< "bysymbol"_n, const_mem_fun< token, uint64_t, &token::get_symbol> > >;

    private:
	token_index tokens;

        void mint(name owner, name ram_payer, asset value, string uri, string name);

        void sub_balance(name owner, asset value);
        void add_balance(name owner, asset value, name ram_payer);
        void sub_supply(asset quantity);
        void add_supply(asset quantity);
};
