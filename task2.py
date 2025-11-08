from websocket import WebSocket
import requests
import time
import json
import argparse

BINANCE = "https://fapi.binance.com"
AGG_TRADES_GET = "/fapi/v1/aggTrades"
PAYLOAD = {"symbol": "BTCUSDT"}

WEBSOCKET = "wss://fstream.binance.com/ws/bnbusdt@aggTrade"

###################### UTILITIES ##############################

# Download a batch of trades
def getTradesSingleShot() -> list[dict]:
    endpoint = BINANCE + AGG_TRADES_GET
    response = requests.api.get(endpoint, params=PAYLOAD)
    trades = response.json()
    return trades

# Parse batch of trades one at a time
def printFormattedTradesSimple(trades: list[dict]) -> None:
    for trade in trades:
        parseSingleTrade(trade)

# Parse a single trade, return the parsing time for further testing 
def parseSingleTrade(unformatted: dict) -> int:
    start = time.perf_counter()
    out = [{
        "a":unformatted["a"],
        "p":unformatted["p"],
        "q":unformatted["q"],
        "f":unformatted["f"],
        "l":unformatted["l"],
        "T":unformatted["T"],
        "m":unformatted["m"],
    }]
    end = time.perf_counter()
    print(json.dumps(out, indent=4))
    print(f"parsing time: {end - start}")
    return end - start

###################### MAIN IMPLEMENTATION ######################

# Parse the aggregate trades using REST API GET request
# This only receives a small batch of the trades at a time after sending a request
def parseTradesSingleShot(iterations: int) -> None:
    count = 0
    while iterations < 0 or count < iterations:
        printFormattedTradesSimple(getTradesSingleShot())
        count += 1

# Parse the aggregate trades stream using WebSocket which does not require sending a GET request for a batch
def parseTradesStream(iterations: int) -> None:
    endpoint = WEBSOCKET
    ws = WebSocket()
    ws.connect(endpoint)

    count = 0
    sum = 0
    while iterations < 0 or count < iterations:
        rawTrade = ws.recv()
        trade = json.loads(rawTrade)
        time = parseSingleTrade(trade)
        
        sum += time
        count += 1
        print(f"running average time parsing is: {sum / count}")


###################### MAIN ######################

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--oneshot", 
                        action="store_true", 
                        help = "use the REST API to download the group of aggregate trades")
    parser.add_argument("--iterations",
                        help = "Number of iterations to download the trade information", default = "-1")
    args = parser.parse_args()
    iters = int(args.iterations)


    if args.oneshot:
        parseTradesSingleShot(iters)
    else:
        parseTradesStream(iters)

