# Non-Fungible Token (NFT) 

For any questions, contributions or feedback join our dedicated telegram group: https://t.me/eosionft

## Simple Summary

A standard interface for non-fungible EOS tokens.

## Abstract

The following standard allows the implementation of a standard API for NFTs within EOS smart contracts. This standard provides basic functionality to track and transfer NFTs.

NFTs can represent ownership over digital or physical assets:

- Physical property — houses, unique artwork
- Cryptocollectibles — unique collectibles or  instances which are part of limited-edition collections. 
- "Negative value" assets — loans, burdens and other responsibilities

NFTs are *distinguishable* and you must track the ownership of each one separately.

## Motivation

A standard interface allows wallet/broker/auction applications to work with any NFT on EOS blockchain. A simple eosio.nft smart contract is provided.

This standard is inspired by the eosio.token standard. eosio.token is insufficient for tracking NFTs because each asset is distinct (non-fungible) whereas each of a quantity of tokens is identical (fungible).

## Specification

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.

``` eosio.nft.hpp
/// @title eosio.nft public interface
/// @dev See https://github.com/jafri/eosio.nft/blob/master/README.md

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

    class nft : public contract {
    public:
        nft(account_name self) : contract(self), tokens(_self, _self) {}

	/// Creates token with a symbol name for the specified issuer account.
	/// Throws if token with specified symbol already exists.
	/// @param issuer Account name of the token issuer
	/// @param symbol Symbol code of the token
        void create(account_name issuer, string symbol);

	/// Issues specified number of tokens with previously created symbol to the account name "to". 
	/// Each token is generated with an unique token_id assigned to it. Requires authorization from the issuer.
	/// Any number of tokens can be issued.
	/// @param to Account name of tokens receiver
	/// @param quantity Number of tokens to issue for specified symbol (positive integer number)
	/// @param uris Vector of URIs for each issued token (size is equal to tokens number)
	/// @param name Name of issued tokens
	/// @param memo Action memo (max. 256 bytes)
        void issue(account_name to,
                   asset quantity,
                   vector<string> uris,
		   string name,
                   string memo);

	/// Transfers 1 token with specified "id" from account "from" to account "to".
	/// Throws if token with specified "id" does not exist, or "from" is not the token owner.
	/// @param from Account name of token owner
	/// @param to Account name of token receiver
	/// @param id Unique ID of the token to transfer
	/// @param memo Action memo (max. 256 bytes)
        void transferid(account_name from,
                      account_name to,
                      id_type id,
                      string memo);

	/// Transfers 1 token with specified symbol in asset from account "from" to account "to".
	/// Throws if amount is not 1, token with specified symbol does not exist, or "from" is not the token owner.
	/// @param from Account name of token owner
	/// @param to Account name of token receiver
	/// @param quantity Asset with 1 token 
	/// @param memo Action memo (max. 256 bytes)
	void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);
		      
	/// @notice Sets owner of the token as a ram payer for stored data.
	/// @param payer Account name of token owner
	/// @param id Unique ID of the token to burn
	void setrampayer(account_name payer, 
			 id_type id);
			 
	/// @notice Burns 1 token with specified "id" owned by account name "owner".
	/// @param owner Account name of token owner
	/// @param id Unique ID of the token to burn
        void burn(account_name owner,
                  id_type token_id);
    
    	/// Structure keeps information about the balance of tokens 
	/// for each symbol that is owned by an account. 
	/// This structure is stored in the multi_index table.
        // @abi table accounts i64
        struct account {
            asset balance;
	    
            uint64_t primary_key() const { return balance.symbol.name(); }
        };

	/// Structure keeps information about the total supply 
	/// of tokens for each symbol issued by "issue" account. 
	/// This structure is stored in the multi_index table.
        // @abi table stat i64
        struct stats {
            asset supply;
            account_name issuer;

            uint64_t primary_key() const { return supply.symbol.name(); }
            account_name get_issuer() const { return issuer; }
        };

	/// Structure keeps information about each issued token.
	/// Each token is assigned a global unique ID when it is issued. 
	/// Token also keeps track of its owner, stores assigned URI and its symbol code.    
	/// This structure is stored in the multi_index table "tokens".
        // @abi table token i64
        struct token {
            id_type id;          // Unique 64 bit identifier,
            uri_type uri;        // RFC 3986
            account_name owner;  // token owner
            asset value;         // token value (1 SYS)
	    string name;	 // token name

            id_type primary_key() const { return id; }
            account_name get_owner() const { return owner; }
            string get_uri() const { return uri; }
            asset get_value() const { return value; }
	    uint64_t get_symbol() const { return value.symbol.name(); }
	    uint64_t get_name() const { return string_to_name(name.c_str()); }
	    
	    uuid get_global_id() const
	    {
		uint128_t self_128 = static_cast<uint128_t>(N(_self));
		uint128_t id_128 = static_cast<uint128_t>(id);
		uint128_t res = (self_128 << 64) | (id_128);
		return res;
	    }

	    string get_unique_name() const
	    {
		string unique_name = name + "#" + std::to_string(id);
		return unique_name;
	    }
        };
	
	/// Account balance table
	/// Primary index:
	///	owner account name
	using account_index = eosio::multi_index<N(accounts), account>;

	/// Issued tokens statistics table
	/// Primary index:	
	///	token symbol name
	/// Secondary indexes:
	///	issuer account name	
	using currency_index = eosio::multi_index<N(stat), stats,
	                       indexed_by< N( byissuer ), const_mem_fun< stats, account_name, &stats::get_issuer> > >;

	/// Issued tokens table
	/// Primary index:
	///	token id
	/// Seconday indexes:
	///	owner account name
	///	token symbol name
	///	issued token name
	using token_index = eosio::multi_index<N(token), token,
	                    indexed_by< N( byowner ), const_mem_fun< token, account_name, &token::get_owner> >,
			    indexed_by< N( bysymbol ), const_mem_fun< token, uint64_t, &token::get_symbol> >,
		            indexed_by< N( byname ), const_mem_fun< token, uint64_t, &token::get_name> > >;
			    
    private:
        friend eosiosystem::system_contract;

	token_index tokens;
    };
} /// namespace eosio
```
In order to query information stored in tables, it is possible to use cleos commands:

display all issued tokens info 

`cleos get table eosio.nft eosio.nft token` 

display "tester1" tokens balance

`cleos get table eosio.nft tester1 accounts`   

or

`cleos get currency balance eosio.nft tester1`

display current supply of tokens with symbol "NFT"

`cleos get table eosio.nft NFT stat`

## To-do
1. Add secondary indices - done
2. Add approval?
3. Transfer between contracts

