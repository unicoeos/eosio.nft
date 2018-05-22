#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Reset
sh ./utility/reset.sh

# All steps
sh ./deployment/1_start.sh
sleep 1

printf "\n\n${GREEN}2. Deploy BIOS${NC}\n"
sh ./deployment/2_deploy_bios.sh
sleep 1

printf "\n\n${GREEN}3. Deploy eosio.token${NC}\n"
sh ./deployment/3_deploy_token.sh
sleep 1

printf "\n\n${GREEN}4. Create and issue SYS${NC}\n"
sh ./deployment/4_create_and_issue.sh
sleep 1

printf "\n\n${GREEN}5. Deploy eosio.system${NC}\n"
sh ./deployment/5_deploy_system.sh
sleep 1

printf "\n\n${GREEN}6. Create Accounts${NC}\n"
sh ./deployment/6_create_account.sh
sleep 1

printf "\n\n${GREEN}6.5 Test eosio.nft${NC}\n"
sh ./deployment/6.5_nft.sh
sleep 1

printf "\n\n${GREEN}7. Register Producer${NC}\n"
sh ./deployment/7_reg_prod.sh
sleep 1

printf "\n\n${GREEN}8. Transfer SYS${NC}\n"
sh ./deployment/8_transfer.sh
sleep 1

printf "\n\n${GREEN}9. Vote Producer${NC}\n"
sh ./deployment/9_vote_producer.sh
sleep 1
