/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio.token/eosio.token.hpp>

namespace eosio {

void token::create( account_name issuer,
                    asset        maximum_supply )
{
    // Print action context for clarity
    // print( "Action: create ", maximum_supply.amount, " ", maximum_supply.symbol, "\n" );

    // Ensure caller has authentication
    require_auth( _self );

    // Ensures valid symbol and valid supply
    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    // Retrieve token statistics table and find existing token
    stats statstable( _self, sym.name() );
    auto existing = statstable.find( sym.name() );

    // Stop if symbol already exists
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    // Rename table attributes (confirm functionality of emplace TODO)
    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}


void token::issue( account_name to, asset quantity, string memo )
{
    // Print action context for clarity
    print( "Action: issue ", quantity.amount, " ", quantity.symbol.name(), " to ", N(to), "\n" );

    // Check if symbol name is valid
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );

    // Retrieve token statistics table
    auto sym_name = sym.name();
    stats statstable( _self, sym_name );

    // Check that token exists
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );

    // Ensure caller has authentication of token issuer
    const auto& st = *existing;
    require_auth( st.issuer );

    // Check for valid quantity attributes
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    // Increase supply of the token by quantity of tokens issued
    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply += quantity;
    });

    // Adds all issued tokens as balance to issuer
    add_balance( st.issuer, quantity, st, st.issuer );

    // If the destination account is not the issuer, send transfer action to that account
    if( to != st.issuer ) {
       SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
    }
}

void token::transfer( account_name from,
                      account_name to,
                      asset        quantity,
                      string       /*memo*/ )
{
    // Print action context for clarity
    print( "Action: transfer from ", eosio::name{from}, " to ", eosio::name{to}, " ", quantity, "\n" );

    // Ensure not a self-transfer
    eosio_assert( from != to, "cannot transfer to self" );

    // Ensure caller has authentication of token sender
    require_auth( from );

    // Ensure account exists
    eosio_assert( is_account( to ), "to account does not exist");

    // Retrieve token statistics table and find current token
    auto sym = quantity.symbol.name();
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );

    // Add the sender and receiver to set of accounts to be notified
    require_recipient( from );
    require_recipient( to );

    // Ensure quantity attributes are valid
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

    // Subtract balance from sender
    sub_balance( from, quantity, st );

    // Add balance to receiver
    add_balance( to, quantity, st, from );
}

void token::sub_balance( account_name owner, asset value, const currency_stats& st ) {
   // Retrieve accounts table for sender
   accounts from_acnts( _self, owner );

   // Find the current token by symbol name
   const auto& from = from_acnts.get( value.symbol.name() );

   // Ensure sender has sufficient balance amount
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

   // Balance is equal to transfer amount
   if( from.balance.amount == value.amount ) {
      from_acnts.erase( from );
   // Balance is larger than transfer amount
   } else {
      from_acnts.modify( from, owner, [&]( auto& a ) {
          a.balance -= value;
      });
   }
}

void token::add_balance( account_name owner, asset value, const currency_stats& st, account_name ram_payer )
{
   // Retrieve accounts table for receiver
   accounts to_acnts( _self, owner );

   // Find the current token by symbol name
   auto to = to_acnts.find( value.symbol.name() );

   // If the token is not found
   if( to == to_acnts.end() ) {
      // Set new token for receiver
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   // If the token is found
   } else {
      // Modify receiver's pre-existing token balance
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

} /// namespace eosio

EOSIO_ABI( eosio::token, (create)(issue)(transfer) )
