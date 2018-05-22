#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m' # No Color

printf "${GREEN}eosio.token${NC}\n"
echo eosio            `cleos get currency balance eosio.token eosio`
echo syedalijafri     `cleos get currency balance eosio.token syedalijafri`
echo producer1111     `cleos get currency balance eosio.token producer1111`

printf "\n${GREEN}eosio.nft${NC}\n"
echo eosio            `cleos get currency balance eosio.token eosio`
echo syedalijafri     `cleos get currency balance eosio.token syedalijafri`
echo producer1111     `cleos get currency balance eosio.token producer1111`
