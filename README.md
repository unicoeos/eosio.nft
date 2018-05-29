# NFT - EOSIO non-fungible token

Welcome to the EOSIO NFT source code repository! This document describes the interface of the non-fungible token - NFT, for EOSIO blockchain.

## Public interface
### CREATE
#### Declaration
	void create(account_name issuer, string symbol);
	
#### Parameters
	issuer - account name of the token issuer
	symbol - symbol code of the token
	
#### Description
	Creates token with symbol name for the issuer account.
	
### ISSUE
#### Declaration
	void issue(account_name to, asset quantity, vector<string> uris, string memo);
	
#### Parameters
	to - account name of tokens receiver
	quantity - number of tokens to issue for specified symbol (positive integer number)
	uris - vector of URIs for each issued token (size is equal to tokens number)
	memo - action memo (max. 256 bytes)
	
#### Description
	Issues specified number of tokens with previousle created symbol to the account name "to". Each token is generated with an               unique token_id assigned to it. 

### TRANSFER
#### Declaration
	void transfer(account_name from, account_name to, id_type id, string memo);
	
#### Parameters
	from - account name of token owner
	to - account name of token receiver
	id - id of the token to transfer 
	memo - action memo (max. 256 bytes)
	
#### Description
	Transfers 1 token with specified "id" from account "from" to account "to".
	
### BURN
#### Declaration
	void burn(account_name owner, id_type token_id);
	
#### Parameters
	owner - account name of token owner
	id - id of the token to burn 
	
#### Description
	Burns 1 token with specified "id" owned by account name 'owner'.
