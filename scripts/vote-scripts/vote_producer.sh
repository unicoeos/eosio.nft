#!/bin/bash

# $1 - voter name
# $2 - list of producer name

cleos push action eosio voteproducer '{"voter":"'"$1"'","proxy":"","producers":['"$2"']}' -p $1