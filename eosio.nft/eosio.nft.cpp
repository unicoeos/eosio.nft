/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "eosio.nft.hpp"

namespace eosio {
    using std::string;

	// @abi action
    void NFT::create( account_name owner, string uri )
    {
        // Have permission to create token
        require_auth( owner );

        // Generate unique token
        uint64_t unique_key = generate_unique_id(owner, tapos_block_prefix());//available_primary_key owner ^ murmur_hash(tapos_block_prefix());


        // Search for pre-existing token with same id
        tokens tokens_table( _self, owner );
        auto itr = tokens_table.find( unique_key );
        eosio_assert( itr == tokens_table.end(), "token with id already exists" );

        tokens_table.emplace( owner, [&]( auto& token ) {
            token.id = unique_key;
            token.uri = uri;
	    token.owner = owner;
        });
    }

	// @abi action
    void NFT::transfer( account_name from,
                        account_name to,
                        uint64_t     id,
                        string       /*memo*/ )
    {
        // Ensure authorized to send from account
        eosio_assert( from != to, "cannot transfer to self" );
        require_auth( from );

        // Ensure 'to' account exists
        eosio_assert( is_account( to ), "to account does not exist");

        // Retrieve table for NFTs owned by 'from'
        tokens sender_tokens( _self, from );

        // Ensure owner owns token
        auto token_itr = sender_tokens.find( id );
        eosio_assert( token_itr != sender_tokens.end(), "sender does not own token with specified ID" );

        // Token Object
        const auto& token_object = *token_itr;

        // Remove token from sender
        sender_tokens.erase(token_itr);

        // Retrieve table for NFTs owned by 'to'
        tokens receiver_tokens( _self, to );

        // Add token to receiver
        receiver_tokens.emplace( from, [&]( auto& token ) {
            token = token_object;
	    token.owner = to;
        });

        // Notify both recipients
        require_recipient( from );
        require_recipient( to );
    }
	
	uint64_t NFT::get_balance( account_name _owner) const
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

EOSIO_ABI( NFT, (create)(transfer) )

} /// namespace eosio

