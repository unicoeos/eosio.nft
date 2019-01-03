cmake_minimum_required(VERSION 3.5)
project(eosio.nft VERSION 1.0.0)

find_package(eosio.cdt)

# add contract
add_contract( eosio.nft eosio.nft eosio.nft.cpp )
