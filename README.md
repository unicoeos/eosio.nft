# Non-Fungible Token (NFT) Proposal on EOS

## Simple Summary

A standard interface for non-fungible EOS tokens.

## Abstract

The following standard allows the implementation of a standard API for NFTs within EOS smart contracts. This standard provides basic functionality to track and transfer NFTs.

NFTs can represent ownership over digital or physical assets:

- Physical property — houses, unique artwork
- Virtual collectables — unique pictures of kittens, collectable cards
- "Negative value" assets — loans, burdens and other responsibilities

In general, all houses are distinct and no two kittens are alike. NFTs are *distinguishable* and you must track the ownership of each one separately.

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
	/// @param memo Action memo (max. 256 bytes)
        void issue(account_name to,
                   asset quantity,
                   vector<string> uris,
                   string memo);

	/// Transfers 1 token with specified "id" from account "from" to account "to".
	/// Throws if token with specified "id" does not exist, or "from" is not the token owner.
	/// @param from Account name of token owner
	/// @param to Account name of token receiver
	/// @param id Unique ID of the token to transfer
	/// @param memo Action memo (max. 256 bytes)
        void transfer(account_name from,
                      account_name to,
                      id_type id,
                      string memo);

	/// @notice Burns 1 token with specified "id" owned by account name "owner".
	/// @param owner Account name of token owner
	/// @param id Unique ID of the token to burn
        void burn(account_name owner,
                  id_type token_id);
    };
} /// namespace eosio
```

