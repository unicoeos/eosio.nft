/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/print.h>
#include <string>
#include <chrono>
#include <ctime>

namespace eosiosystem {
    class system_contract;
}

namespace eosio {
    using std::string;
    typedef uint64_t uuid;

    class NFT : public contract {
    public:
        NFT( account_name self ):contract(self), tokens(_self, _self){}

        void create( account_name owner, string symbol, string uri );

        void transfer( account_name from,
                       account_name to,
                       uuid         id,
                       string       memo );

	// returns number of all tokens owned by _owner
	// uint64_t get_balance( account_name _owner) const;

	// return token owner by token id
	// account_name get_owner(uint64_t tokenId) const;

	// approve ownerhip of the token by tokenId
	//void approve(account_name approved, uint64_t tokenId);

	// approve ownership of all tokens
	//void set_approval_for_all(account_name account, bool approved);

	// return approved account of the token
	//account_name get_approved(uint64_t tokenId) const;

	// check if the account is approved for all owner tokens
	//bool is_approved_for_all(account_name owner, account_name account) const;

	//uint64_t total_supply() const;

    private:
        friend eosiosystem::system_contract;

        // @abi table token i64
        struct token {
            uuid           id;     // Unique 64 bit identifier,
            string         uri;    // RFC 3986
	        account_name   owner;  // token owner
            string         symbol; // token symbol

            auto primary_key() const { return id; }
            /// auto get_account() const { return owner; }
            // auto get_symbol() const { return symbol; }
        };

        typedef eosio::multi_index<N(token), token> token_index;
        token_index tokens;

    };

    inline static uuid generate_unique_id( account_name owner, int tapos_prefix ){

        uint32_t accHash = std::hash<account_name>{}(owner);

        uint32_t taposHash = std::hash<int>{}(tapos_prefix);
        uint32_t accTaposXOR = accHash ^ taposHash;

        print("Time right now() is: ", current_time(), "       ");

        uint32_t timeHash = std::hash<uint64_t>{}(static_cast<uint64_t>(current_time()));
        print("Time hash is: ", timeHash, "       ");

        uint32_t accTimeXOR = accHash ^ timeHash;

        uint64_t accTaposXOR64 = static_cast<uint64_t>(accTaposXOR);
        uint64_t accTimeXOR64 = static_cast<uint64_t>(accTimeXOR);
        uuid resHash = (accTaposXOR64 << 32) | (accTimeXOR64);
        return resHash;
    }

} /// namespace eosio
