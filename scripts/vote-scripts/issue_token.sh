#!/bin/bash

# $1 - account_name of requiring tokens
# $2 - token amount required

cleos push action eosio.token issue '["'"$1"'","'"$2"' SYS","issue '"$2"' SYS to '"$1"'"]' -p eosio
