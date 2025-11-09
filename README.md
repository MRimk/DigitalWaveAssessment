# Tasks for Digital Wave Futures

## Task 1

### Compilation

**Requirement**: please install `libcurl4-openssl-dev` to use curl in C++ and download the textfile that is then parsed.

Run `make all` to compile the task1 C++ file. This will compile the code that runs and downloads the provided textfile, and uses it to fill the hashmap.

To test the task1, I wrote a small test in `main()` function that can be compiled with `make test`. It's quite rudimentary but it shows the hashmap functionality.

### Hash table (QuickHashMap.hpp)

To have hash table, I use hash function modulo the size of the table. However, since modulo can create more hash collisions (hash function should only collide for the same input without hash - i.e. iff key1 == key2, then hash(key1) == hash(key2)), we need to handle the case where different keys would have the same hash.

This is where linear probing is implemented using tombstones list. Tombstone[i] is intended to show that the slot that was used before, is not empty and can be used for probing. At start-up no slot has ever held an entry, so no slot is a tombstone yet (every position is a "hard empty" cell). Therefore the initial value must be false.

Insert / Remove / Get are O(1) complexity if the key does not collide with other keys using the modulo hash. To have that, the size of the table has to be big enough for the hash not do have collisions. 

### Quick access last / least recent modification

To have O(1) access of the least recent and last modification, I implemented a doubly linked list. In this list, the ends are the ones pointed to by the earliest / latest pointers, and the other nodes are accessible simply by the pointers stored in the hash table.

Removing the item from this list is simply reconnecting the neighbors, and similarly is reconnecting the item as the last modified where only the latest neighbors are added.

### Text download

I implemented the text download from the given URL (https://www.gutenberg.org/files/98/98-0.txt) using curl in C++. I had never used it in code before, but since I knew its behaviour in CLI, I figured I would learn by following a tutorial and using it here. I have linked a tutorial in the code, but expanded it to include the stream word parsing and putting those words in the hashmap.

## Task 2

### Assumptions

1. There was no symbol specified for the GET request, i.e. there was no trade from-to specified. Therefore I picket BTC to USDT.
2. Stream of trades means endlessly many trades
3. The user is able to set up a python environment (required for python 3.12 and above) and do `pip install requests websocket-client`.

### Parsing speed

Single trade parsing is just python dictionary item access which is O(1). After printing all the times for each trade, I did confirm that they all take the same amount of time (within 1e-07s from each other).

### Implementation

#### One shot (repeated) REST API

First implementation is using GET endpoint shown in the assignment description. This GET request returns a batch of trades, which then are parsed one by one. To create a stream, a while loop is run in which GET request is repeated.

#### Stream receive using WebSocket

Second implementation is using WebSocket API described in [Aggregate Trade Streams API](https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams/Aggregate-Trade-Streams). This allows us to repeatedly call `recv()` on the websocket and therefore get one trade at a time.

This approach is cleaner as we do not need to parse more trades in one batch, or send many requests, since websocket is already connected to the endpoint.

#### Options and defaults

I implemented a selection between the stream implementations using `--oneshot` option, which selects the REST API approach. By default (if no option is provided), the script runs the WebSocket approach.

Next, I implemented iterations limitting using `--iterations=` option, which by default is -1 to run endlessly. If specified, the script will run up to the number of iterations provided.