/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "eosio.nft.hpp"

namespace eosio {
    using std::string;
    using eosio::asset;

    // @abi action
    void nft::create( account_name issuer, string sym ) {
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
    void nft::issue( account_name to,
                     asset quantity,
                     string memo,
                     vector<string> uris )
    {
        // e,g, Get EOS from 3 EOS
        symbol_type symbol = quantity.symbol;
        eosio_assert( symbol.is_valid(), "invalid symbol name" );
        eosio_assert( symbol.precision() == 1, "quantity must be a while number" );
        eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

        // Ensure currency has been created
        currency_index currency_table( _self, symbol );
        auto existing_currency = currency_table.find( symbol );
        eosio_assert( existing_currency != currency_table.end(), "token with symbol does not exist, create token before issue" );
        const auto& st = *existing_currency;

        // Ensure have issuer authorization and valid quantity
        require_auth( st.issuer );
        eosio_assert( quantity.is_valid(), "invalid quantity" );
        eosio_assert( quantity.amount > 0, "must issue positive quantity of NFTs" );
        eosio_assert( symbol == st.supply.symbol, "symbol precision mismatch" );

        // Increase supply
        currency_table.modify( st, 0, [&]( auto& currency ) {
            currency.supply += quantity;
        });

        // Check that number of tokens matches uri size
        eosio_assert( quantity.amount == uris.size(), "mismatch between number of tokens and uris provided" );

        // Mint nfts
        for(auto const& uri: uris) {
            mint( to, asset{ 1, symbol }, uri);
        }

        // Add balance to account
        account_index to_table( _self, to );
        const auto& to_account = to_table.get( symbol, "no balance object found" );

        to_table.modify( to_account, 0, [&]( auto& account ) {
            account.balance += quantity;
        });

    }

    // @abi action
    void nft::transfer( account_name from,
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
        auto sender_token = tokens.find( id );
        eosio_assert( sender_token != tokens.end(), "sender does not own token with specified ID" );
        const auto& st = *sender_token;

        // Notify both recipients
        require_recipient( from );
        require_recipient( to );

        // Check memo size and print
        eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

        // Transfer NFT from sender to receiver
        tokens.modify( st, from, [&]( auto& token ) {
	        token.owner = to;
        });

        // Change balance of both accounts
        symbol_type symbol = st.value.symbol.name();

        account_index from_table( _self, from );
        const auto& from_account = from_table.get( symbol, "no balance object found" );
        account_index to_table( _self, to );
        const auto& to_account = to_table.get( symbol, "no balance object found" );

        from_table.modify( from_account, 0, [&]( auto& account ) {
            account.balance -= st.value;
        });

        to_table.modify( to_account, 0, [&]( auto& account ) {
            account.balance += st.value;
        });

    }

    void nft::mint( account_name owner,
                    asset value,
                    string uri)
    {
        // Add token with creator paying for RAM
        tokens.emplace( owner, [&]( auto& token ) {
            token.id = tokens.available_primary_key();
            token.uri = uri;
            token.owner = owner;
            token.value = value;
        });
    }


    void nft::burn( account_name owner, id_type token_id, string sym  )
    {
        require_auth( owner );

        // Token table
        // Find token to burn
        auto burn_token = tokens.find( token_id );
        eosio_assert( burn_token->owner == owner, "token not owned by you" );

        // Remove token from tokens table
        tokens.erase( burn_token );


        // Currency Table
        // Ensure symbol is valid
        symbol_type symbol = string_to_symbol(0, sym.c_str());
        eosio_assert( symbol.is_valid(), "invalid symbol name" );

        // Create asset
        asset burnt_supply(1, symbol);

        // Remove supply
        currency_index currency_table( _self, symbol );
        auto current_currency = currency_table.find( symbol );

        currency_table.modify( current_currency, 0, [&]( auto& currency ) {
            currency.supply -= burnt_supply;
        });


        // Account table
        // Lower supply
        account_index account_table( _self, owner );
        const auto& from = account_table.get( symbol.name(), "no balance object found" );

        account_table.modify( from, 0, [&]( auto& account ) {
            account.balance -= burnt_supply;
        });

    }

EOSIO_ABI( nft, (create)(issue)(transfer)(burn) )

} /// namespace eosio
