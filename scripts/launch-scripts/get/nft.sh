#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m' # No Color

printf "\n${GREEN}eosio.nft${NC}\n"
echo syedalijafri     `cleos get table eosio.nft syedalijafri tokens`

echo syedalijafri balance $(cleos push action eosio.nft owns "['syedalijafri']"`
