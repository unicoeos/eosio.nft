#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m' # No Color

printf "\n${GREEN}1. Create eosio.nft account${NC}\n"
cleos create account eosio eosio.nft EOS6QahzfDoDvdXGTjV8YWxZ6vEYHqxen9v16U1aRZj7kWiPpmb4E EOS6QahzfDoDvdXGTjV8YWxZ6vEYHqxen9v16U1aRZj7kWiPpmb4E
sleep 1

printf "\n${GREEN}2. Transfer money to eosio.nft${NC}\n"
cleos transfer syedalijafri eosio.nft "200000.0000 SYS"
sleep 1

printf "\n${GREEN}3. Stake for Bandwidth${NC}\n"
cleos system delegatebw syedalijafri syedalijafri "200000.0000 SYS" "200000.0000 SYS" --transfer
sleep 1

printf "\n${GREEN}4. Deploy eosio.nft${NC}\n"
cleos set contract eosio.nft $EOS_BUILD_DIR/contracts/eosio.nft -p eosio.nft
sleep 1

printf "\n${GREEN}5. Create NFT${NC}\n"
cleos push action eosio.nft create '["syedalijafri", "eos.cafe"]' -p syedalijafri
