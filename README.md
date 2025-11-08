# Tasks for Digital Wave Futures

## Task 1

### Hash table

To have hash table, I use hash function modulo the size of the table. However, since modulo can create more hash collisions (hash function should only collide for the same input without hash - i.e. iff key1 == key2, then hash(key1) == hash(key2)), we need to handle the case where different keys would have the same hash.

This is where linear probing is implemented using tombstones list. Tombstone[i] is intended to show that the slot that was used before, is not empty and can be used for probing. At start-up no slot has ever held an entry, so no slot is a tombstone yet (every position is a "hard empty" cell). Therefore the initial value must be false.

Insert / Remove / Get are O(1) complexity if the key does not collide with other keys using the modulo hash. To have that, the size of the table has to be big enough for the hash not do have collisions. 

### Quick access last / least recent modification

To have O(1) access of the least recent and last modification, I implemented a doubly linked list. In this list, the ends are the ones pointed to by the earliest / latest pointers, and the other nodes are accessible simply by the pointers stored in the hash table.

Removing the item from this list is simply reconnecting the neighbors, and similarly is reconnecting the item as the last modified where only the latest neighbors are added.