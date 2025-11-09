#ifndef QUICK_HASH_MAP_HPP
#define QUICK_HASH_MAP_HPP

#include <string>
#include <vector>
#include <memory>

struct OrderList {
	std::string word;
	int value;
	OrderList* prev;
	OrderList* next;

	OrderList(const std::string& w, int v) : word(w), value(v), prev(nullptr), next(nullptr) {}

};

class QuickHashMap {
public:
    explicit QuickHashMap(size_t size);

    void insert(const std::string& key, int value);
    void remove(const std::string& key);
    int get(const std::string& key) const;

    std::string getLast() const;
    std::string getEarliest() const;

private:
    // linear-probe until we find key or first reusable slot.
    size_t probe(const std::string& key, bool& found) const;

    // detach a node from the doubly-linked list.
    void detach(OrderList* node);

    // update node to the latest of the usage list.
    void makeLatest(OrderList* node);

    std::hash<std::string>                 _hasher;
    const size_t                           _size;

    std::vector<std::shared_ptr<OrderList>> _array;
    std::vector<bool> _tombstone; // list of slots that can be used

    OrderList* _latest   {nullptr}; // most recently touched
    OrderList* _earliest {nullptr}; // least recently touched
};

#endif