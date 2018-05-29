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
	uris - vector of URIs for each issued token (size equals to tokens number)
	memo - action memo (max. 256 bytes)
	
#### Description
	Issues specified number of tokens with previousle created symbol to the account name "to". Each token is generated with an unique         id assigned to it. 

