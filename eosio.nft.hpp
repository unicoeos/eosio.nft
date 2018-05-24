/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>

namespace eosiosystem {
    class system_contract;
}

namespace eosio {
    using std::string;
    typedef uint128_t uuid;
    typedef uint64_t id_type;
    typedef string uri_type;

    class NFT : public contract {
    public:
        NFT(account_name self) : contract(self), tokens(_self, _self) {}

        void create( account_name issuer, string symbol );

        void issue( account_name to,
                    asset quantity,
                    string memo,
                    int num_of_tokens,
                    vector<std::string> uris );

        void transfer(account_name from,
                      account_name to,
                      id_type      id,
                      string       memo );

    private:
        friend eosiosystem::system_contract;

        // @abi table accounts i64
        struct account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.name(); }
        };

        // @abi table stat i64
        struct stats {
            asset supply;
            account_name issuer;

            uint64_t primary_key() const { return supply.symbol.name(); }
            account_name get_issuer() const { return issuer; }
        };

        // @abi table token i64
        struct token {
            id_type id;          // Unique 64 bit identifier,
            uri_type uri;        // RFC 3986
            account_name owner;  // token owner
            asset value;         // token value (1 SYS)

            auto primary_key() const { return id; }
            uuid get_global_id() const { return N(_self ) * id; }
            auto get_account() const { return owner; }
            auto get_uri() const { return uri; }
            auto get_value() const { return value; }
        };

        typedef eosio::multi_index<N(accounts), account> account_index;
        typedef eosio::multi_index<N(stat), stats> currency_index;
        typedef eosio::multi_index<N(token), token> token_index;
        token_index tokens;

        void create_tokens(account_name owner, asset value, int num_of_tokens, vector<std::string> uris);
    };
} /// namespace eosio
