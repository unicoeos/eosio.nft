/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <string>
#include <chrono>
#include <ctime>

namespace eosiosystem {
    class system_contract;
}

namespace eosio {

    using std::string;

    class NFT : public contract {
    public:
        NFT( account_name self ):contract(self){}
        
        void create( account_name owner, string uri );

        void transfer( account_name from,
                       account_name to,
                       uint64_t     id,
                       string       memo );
		
		// returns number of all tokens owned by _owner 
	uint64_t get_balance( account_name _owner) const;	
		// return token owner by token id
	account_name get_owner(uint64_t tokenId) const;
		
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
            uint64_t       id;  // Unique 64 bit identifier,
            string         uri;   // RFC 3986
	    account_name   owner; // token owner

            auto primary_key() const { return id; }
        };

        typedef eosio::multi_index<N(tokens), token> tokens;
    };



    /* token NFT::get_balance( account_name owner )const
     {
         // Ensure 'owner' account exists
         eosio_assert( is_account( owner ), "owner account does not exist");

         // Retrieve table for NFTs
         tokens tokens_table( _self, owner );
         auto owned = tokens_table.find( owner );

         // Ensure token exists
         return tokens_table;
     }*/


    /*inline static uint64_t murmur_hash( const int& key ){
        return std::hash<int>{}(key);
    }*/

    inline static uint64_t generate_unique_id( account_name owner, int tapos_prefix ){

        uint32_t accHash = std::hash<account_name>{}(owner);

        uint32_t taposHash = std::hash<int>{}(tapos_prefix);
        uint32_t accTaposXOR = accHash ^ taposHash;

	auto currTime = time(0);//now();
        auto epochCount = currTime;//currTime.time_since_epoch().count();
	
        print("Time right now() is: ", epochCount, "       ");
       
	uint32_t timeHash = std::hash<uint64_t>{}(static_cast<uint64_t>(epochCount));
	print("Time hash is: ", timeHash, "       ");

        //int64_t test = eosio::time_point::now();
        //print("Time Hash milliseconds: ", test);
        uint32_t accTimeXOR = accHash ^ timeHash;

        uint64_t accTaposXOR64 = static_cast<uint64_t>(accTaposXOR);
        uint64_t accTimeXOR64 = static_cast<uint64_t>(accTimeXOR);
        uint64_t resHash = (accTaposXOR64 << 32) | (accTimeXOR64);
        return resHash;
    }

} /// namespace eosio
