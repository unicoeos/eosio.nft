# Usage: cleos get table [OPTIONS] contract scope table

cleos get table eosio eosio producerinfo | jq ".rows[0].owner"
cleos get table eosio eosio producerinfo | jq ".rows[0].total_votes"

cleos get table eosio eosio producerinfo | jq ".rows[1].owner"
cleos get table eosio eosio producerinfo | jq ".rows[1].total_votes"

cleos get table eosio eosio producerinfo | jq ".rows[2].owner"
cleos get table eosio eosio producerinfo | jq ".rows[2].total_votes"
