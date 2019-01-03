# Updated to EOSIO.CDT v1.4.0

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

	/// Creates token with a symbol name for the specified issuer account.
	/// Throws if token with specified symbol already exists.
	/// @param issuer Account name of the token issuer
	/// @param symbol Symbol code of the token
        ACTION create(name issuer, std::string symbol);

	/// Issues specified number of tokens with previously created symbol to the account name "to". 
	/// Each token is generated with an unique token_id assigned to it. Requires authorization from the issuer.
	/// Any number of tokens can be issued.
	/// @param to Account name of tokens receiver
	/// @param quantity Number of tokens to issue for specified symbol (positive integer number)
	/// @param uris Vector of URIs for each issued token (size is equal to tokens number)
	/// @param name Name of issued tokens (max. 32 bytes)
	/// @param memo Action memo (max. 256 bytes)
        ACTION issue(name to,
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
        ACTION transferid(name from,
                      name to,
                      id_type id,
                      string memo);

	/// Transfers 1 token with specified symbol in asset from account "from" to account "to".
	/// Throws if amount is not 1, token with specified symbol does not exist, or "from" is not the token owner.
	/// @param from Account name of token owner
	/// @param to Account name of token receiver
	/// @param quantity Asset with 1 token 
	/// @param memo Action memo (max. 256 bytes)
	ACTION transfer(name from,
                      name to,
                      asset quantity,
                      string memo);

	/// @notice Burns 1 token with specified "id" owned by account name "owner".
	/// @param owner Account name of token owner
	/// @param id Unique ID of the token to burn
	ACTION burn(name owner,
                  id_type token_id);

	/// @notice Sets owner of the token as a ram payer for stored data.
	/// @param payer Account name of token owner
	/// @param id Unique ID of the token to burn
        ACTION setrampayer(name payer, 
			   id_type id);
    
    	/// Structure keeps information about the balance of tokens 
	/// for each symbol that is owned by an account. 
	/// This structure is stored in the multi_index table.
        TABLE account {

            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

	/// Structure keeps information about the total supply 
	/// of tokens for each symbol issued by "issue" account. 
	/// This structure is stored in the multi_index table.
        TABLE stats {
            asset supply;
            name issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
            uint64_t get_issuer() const { return issuer.value; }
        };

	/// Structure keeps information about each issued token.
	/// Each token is assigned a global unique ID when it is issued. 
	/// Token also keeps track of its owner, stores assigned URI and its symbol code.    
	/// This structure is stored in the multi_index table "tokens".
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
	    // contract name, passed as argument
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
	
	/// Account balance table
	/// Primary index:
	///	owner account name
	using account_index = eosio::multi_index<"accounts"_n, account>;

	/// Issued tokens statistics table
	/// Primary index:	
	///	token symbol name
	/// Secondary indexes:
	///	issuer account name	
	using currency_index = eosio::multi_index<"stat"_n, stats,
	                       indexed_by< "byissuer"_n, const_mem_fun< stats, uint64_t, &stats::get_issuer> > >;

	/// Issued tokens table
	/// Primary index:
	///	token id
	/// Seconday indexes:
	///	owner account name
	///	token symbol name
	using token_index = eosio::multi_index<"token"_n, token,
	                    indexed_by< "byowner"_n, const_mem_fun< token, uint64_t, &token::get_owner> >,
			    indexed_by< "bysymbol"_n, const_mem_fun< token, uint64_t, &token::get_symbol> > >;
			    
    private:
        token_index tokens;
};
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

Build command for EOSIO.CDT v1.4.0

`eosio-cpp -o eosio.nft.wasm eosio.nft.cpp --abigen --contract nft`

## How to run unit tests
1. Download the ["eosio.contracts"](https://github.com/EOSIO/eosio.contracts) repository and follow instructions on how to build it.
2. Clone the **"eosio.nft"** repository into the **"eosio.contracts"** folder
3. Add the following line to CMakeLists.txt in the **"eosio.contracts"** folder
```	
	...
	add_subdirectory(eosio.token)
	add_subdirectory(eosio.nft)   <-- add this
	...
```	
4. Copy files (**"contractshpp.hpp.in"** and **"eosio.nft_tests.cpp"**) from the **"eosio.contracts/eosio.nft/tests"** folder to **"eosio.contracts/tests"**
5. Rebuild the **"eosio.contracts"**
6. Copy the file **"eosio.nft.abi"** from **"eosio.contracts/eosio.nft"** to **"eosio.contracts/build/eosio.nft"**
7. Go to the **"eosio.contracts/build/tests"** folder and run the following command

`./unit_test -t eosio_nft_tests`

## To-do
1. Add secondary indices - done
2. Add approval?
3. Transfer between contracts

