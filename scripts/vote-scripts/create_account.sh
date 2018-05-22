#!/bin/bash
# $1 - account name
# $2 - public key
# $3 - private key

cleos system newaccount eosio $1 $2 $2 --stake-net "0.1000 SYS" --stake-cpu "0.1000 SYS" --buy-ram-EOS "0.1000 SYS"
cleos wallet import $3
