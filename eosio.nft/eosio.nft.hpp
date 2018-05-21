/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>

#include <string>

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

      private:
         friend eosiosystem::system_contract;

       // @abi table token i64
       struct token {
             uint64_t       id;  // Unique 64 bit identifier,
             string         uri; // RFC 3986
             account_name   owner;

             uint64_t primary_key()const { return id; }
             account_name get_owner()const { return owner; }

             EOSLIB_SERIALIZE( token, (id)(uri)(owner) )
         };

         typedef eosio::multi_index<N(tokens), token,
                        indexed_by< N(byowner), const_mem_fun<token, account_name, &token::get_owner> > tokens;

         inline token get_balance( account_name owner )const;
   };


    /*
    token NFT::get_balance( account_name owner )const
    {
        // Ensure 'owner' account exists
        eosio_assert( is_account( owner ), "owner account does not exist");

        // Retrieve table for NFTs
        tokens tokens_table( _self, owner );
        auto owned = tokens_table.find( owner );

        // Ensure token exists
        return tokens_table;
    }
    */

   inline static uint64_t murmur_hash( const int& key ){
       return std::hash<int>{}(key);
   }

} /// namespace eosio
