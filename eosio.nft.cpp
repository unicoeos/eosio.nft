/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "eosio.nft.hpp"

namespace eosio {
    using std::string;

    // @abi action
    void NFT::create( account_name owner, string symbol, string uri)
    {
        // Have permission to create token
        require_auth( owner );

        // Generate unique token
        uuid unique_key = generate_unique_id(owner, tapos_block_prefix());

        // Search for pre-existing token with same id
        auto itr = tokens.find( unique_key );
        eosio_assert( itr == tokens.end(), "token with id already exists" );

        // Add token with creator paying for RAM
        tokens.emplace( owner, [&]( auto& token ) {
            token.id = unique_key;
            token.owner = owner;
            token.symbol = symbol;
            token.uri = uri;
        });
    }

    // @abi action
    void NFT::transfer( account_name from,
                        account_name to,
                        uuid     id,
                        string       memo )
    {
        // Ensure authorized to send from account
        eosio_assert( from != to, "cannot transfer to self" );
        require_auth( from );

        // Ensure 'to' account exists
        eosio_assert( is_account( to ), "to account does not exist");

        // Ensure owner owns token
        auto sender_tokens = tokens.find( id );
        eosio_assert( sender_tokens != tokens.end(), "sender does not own token with specified ID" );

        // Notify both recipients
        require_recipient( from );
        require_recipient( to );

        // Check memo size and print
        eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

        // Transfer NFT from sender to receiver
        tokens.modify( sender_tokens, from, [&]( auto& token ) {
	        token.owner = to;
        });
    }

    /*
    uuid NFT::get_balance( account_name _owner) const
    {
	// Ensure '_owner' account exists
	eosio_assert( is_account( _owner ), "_owner account does not exist");

	// Retrieve table for NFTs owned by '_owner'
	tokens owner_tokens( _self, _owner );

	uint64_t tokensNumber = 0;
	for(auto it=owner_tokens.begin(); it!=owner_tokens.end();++it)
		tokensNumber++;

	return tokensNumber;
    }

    account_name NFT::get_owner( uint64_t id ) const
    {
	tokens all_tokens ( _self, _self );
	auto token_itr = all_tokens.find( id );
	eosio_assert( token_itr != all_tokens.end(), "owner does not exist for token with specified ID" );

	return token_itr->owner;
    }
    */

EOSIO_ABI( NFT, (create)(transfer) )

} /// namespace eosio
