#cleos push action eosio delegatebw '{"from":"voter","receiver":"voter","stake_net_quantity":"23.0000 EOS","stake_cpu_quantity":"20.0000 EOS","stake_storage_quantity":"1.0000 EOS"}' -p voter
#cleos push action eosio voteproducer '["voter", "", ["producer1"]]' -p voter
#cleos system voteproducer prods voter producer1 -p voter

cleos system delegatebw syedalijafri syedalijafri "100000000.0000 SYS" "50000000.0000 SYS" --transfer
cleos system voteproducer prods syedalijafri eosio

#cleos push action eosio voteproducer '["xiaohua", "", ["frank", "producer"]]' -p xiaohua
