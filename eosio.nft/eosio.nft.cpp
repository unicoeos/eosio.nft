/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio.nft/eosio.nft.hpp>

namespace eosio {
    using std::string;

    // @abi action
    void NFT::create( account_name owner, string uri )
    {
        // Have permission to create token
        require_auth( owner );

        // Generate unique token
        uint64_t unique_key = available_primary_key owner ^ murmur_hash(tapos_block_num());

        // DEBUG
        print(unique_key);

        // Search for pre-existing token with same ID
        tokens tokens_table( _self, unique_key );
        auto itr = tokens_table.find( unique_key );
        eosio_assert( itr == tokens_table.end(), "token with ID already exists" );

        tokens_table.emplace( _self, [&]( auto& token ) {
            token.id = tokens_table.available_primary_key();
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

        // Retrieve table for NFTs
        tokens tokens_table( _self, from );

        // Ensure token exists
        auto existing = tokens_table.find( id );
        eosio_assert( existing != tokens_table.end(), "token with ID does not exist" );
        const auto& ut = *existing;

        // Ensure token owner matches 'from'
        eosio_assert( ut.owner == from, "token owner does not match sender" );

        // Notify both recipients
        require_recipient( from );
        require_recipient( to );

        // Perform transfer
        tokens_table.modify( ut, 0, [&]( auto& token ) {
            token.owner = to;
        });
    }
} /// namespace eosio

EOSIO_ABI( eosio::NFT, (create)(transfer) )
