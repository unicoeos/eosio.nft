#!/bin/bash

# $1 - from
# $2 - receiver
# $3 - net stake
# $4 - cpu stake

cleos system delegatebw $1 $2 "$3 SYS" "$4 SYS" --transfer
