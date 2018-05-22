#!/bin/bash
# Kills all nodeos processes
kill -9 $(netstat -peanut | grep nodeos | awk '{print $9}' | sed -e 's/^\(.*\)\/.*$/\1/' | uniq)

# Kills all keosd processes
kill -9 $(netstat -peanut | grep keosd | awk '{print $9}' | sed -e 's/^\(.*\)\/.*$/\1/' | uniq)

# Remove pids
rm -rf eos.pid wallet.pid

# Clear all data
sh ./utility/clear.sh

echo "Nodeos and keosd processes killed. Logs and data removed."
