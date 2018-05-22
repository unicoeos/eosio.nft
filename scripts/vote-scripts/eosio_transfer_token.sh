#!/bin/bash

# $1 - account_name of requiring tokens
# $2 - token amount required

cleos push action eosio.token transfer '["eosio","'"$1"'","'"$2"' SYS","transfer '"$2"' SYS to '"$1"'"]' -p eosio


