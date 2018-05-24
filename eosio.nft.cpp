/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "eosio.nft.hpp"

namespace eosio {
    using std::string;
    using eosio::asset;

    // @abi action
    void NFT::create( account_name issuer, string sym ) {
        require_auth( _self );

        // Valid symbol
        asset supply(0, string_to_symbol(0, sym.c_str()));

        auto symbol = supply.symbol;
        eosio_assert( symbol.is_valid(), "invalid symbol name" );
        eosio_assert( supply.is_valid(), "invalid supply");

        // Check if currency with symbol already exists
        currency_index currency_table( _self, symbol.name() );
        auto existing_currency = currency_table.find( symbol.name() );
        eosio_assert( existing_currency == currency_table.end(), "currency with symbol already exists" );

        // Create new currency
        currency_table.emplace( _self, [&]( auto& currency ) {
           currency.supply = supply;
           currency.issuer = issuer;
        });
    }

    // @abi action
    void NFT::issue( account_name to,
                     asset quantity,
                     string memo,
                     int num_of_tokens,
                     vector<std::string> uris )
    {
        // e,g, Get EOS from 3 EOS
        auto symbol = quantity.symbol;
        eosio_assert( symbol.is_valid(), "invalid symbol name" );
        eosio_assert( symbol.precision() == 1, "quantity must be a while number" );
        eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

        // Ensure currency has been created
        auto symbol_name = symbol.name();
        currency_index currency_table( _self, symbol_name );
        auto existing_currency = currency_table.find( symbol_name );
        eosio_assert( existing_currency != currency_table.end(), "token with symbol does not exist, create token before issue" );
        const auto& st = *existing_currency;

        // Ensure have issuer authorization and valid quantity
        require_auth( st.issuer );
        eosio_assert( quantity.is_valid(), "invalid quantity" );
        eosio_assert( quantity.amount > 0, "must issue positive quantity of NFTs" );
        eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

        // Increase supply
        currency_table.modify( st, 0, [&]( auto& currency ) {
            currency.supply += quantity;
        });

        /*  add_balance( st.issuer, quantity, st.issuer );  */
    }

    // @abi action
    void NFT::transfer( account_name from,
                        account_name to,
                        id_type      id,
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

    void NFT::create_tokens( account_name owner,
                             asset value,
                             int num_of_tokens,
                             vector<std::string> uris )
    {
        // Check that number of tokens matches uri size
        eosio_assert( num_of_tokens == uris.size(), "mismatch between number of tokens and uris provided" );

        // For each uri provided, create token
        for(auto const& uri: uris) {
            // Add token with creator paying for RAM
            tokens.emplace( owner, [&]( auto& token ) {
                token.id = tokens.available_primary_key();
                token.uri = uri;
                token.owner = owner;
                token.value = value;
            });
        }
    }

EOSIO_ABI( NFT, (create)(issue)(transfer) )

} /// namespace eosio
