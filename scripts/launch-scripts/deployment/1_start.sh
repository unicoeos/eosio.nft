GREEN='\033[0;32m'
NC='\033[0m' # No Color
mkdir -p logs-dir

echo "${GREEN}Starting Nodeos${NC}"
nohup nodeos --data-dir ./data-dir --config-dir ./config-dir  > ./logs-dir/eos.log 2>&1 &
echo $! > eos.pid

echo "${GREEN}Starting keosd wallet${NC}"
nohup keosd -d ./data-dir --http-server-address localhost:8900 >./logs-dir/wallet.log 2>&1 &
echo $! > wallet.pid
