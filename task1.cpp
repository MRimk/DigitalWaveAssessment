#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

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


QuickHashMap::QuickHashMap(size_t size)
    : _size(size),
      _array(size, nullptr),
      _tombstone(size, false) {}

size_t QuickHashMap::probe(const std::string& key, bool& found) const {
    size_t start = _hasher(key) % _size;
    size_t firstEmpty = SIZE_MAX; // to compare first reusable slot

	// set found if the key already exists
    found = false;

    for (size_t i = 0; i < _size; ++i) {
		// circular traverse
        size_t idx = (start + i) % _size;

        if (_array[idx]) {
			// if key already exists
            if (_array[idx]->word == key) {
                found = true;
                return idx; 
            }
        } else {
            // remember the first reusable spot (empty or tombstone)
            if (firstEmpty == SIZE_MAX) firstEmpty = idx;
            if (!_tombstone[idx]) break; // key won't appear beyond here, so stop
        }
    }

    if (firstEmpty == SIZE_MAX)
        throw std::overflow_error("QuickHashMap is full");

    return firstEmpty; // key absent
}

void QuickHashMap::detach(OrderList* node) {
    if (!node) return;

	// connect neighbors
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

	// update quick access
    if (_earliest == node) _earliest = node->next;
    if (_latest == node) _latest = node->prev;
}

void QuickHashMap::makeLatest(OrderList* node) {
    if (!node || _latest == node) return;

	// first insertion
    if (!_latest) {
        _latest = _earliest = node;
        node->prev = node->next = nullptr;
        return;
    }

    // if already somewhere in list, detach first.
    detach(node);

    // append after current latest
    node->prev     = _latest;
    node->next     = nullptr;
    _latest->next  = node;
    _latest        = node;
}

void QuickHashMap::insert(const std::string& key, int value) {
    bool found = false;
    size_t pos = probe(key, found);

    std::shared_ptr<OrderList> node;

    if (found) {
        // update existing
        node = _array[pos];
        node->value = value;
    } else {
        // create new
        node = std::make_shared<OrderList>(key, value);
        _array[pos] = node;
        _tombstone[pos] = false;
    }

    makeLatest(node.get());
}

void QuickHashMap::remove(const std::string& key) {
    bool found = false;
    size_t pos = probe(key, found);
    if (!found) return;

    std::shared_ptr<OrderList> item = _array[pos];

    // unlink from usage list first
    detach(item.get());

    _array[pos].reset();
    _tombstone[pos] = true;

    // if list became empty
    if (!_latest) _earliest = nullptr;
}

int QuickHashMap::get(const std::string& key) const {
    bool found = false;
    size_t pos = probe(key, found);
    return found ? _array[pos]->value : -1;
}

std::string QuickHashMap::getLast() const {
    return _latest ? _latest->word : "";
}

std::string QuickHashMap::getEarliest() const {
    return _earliest ? _earliest->word : "";
}

int main() {
    QuickHashMap map(300);
    map.insert("hello", 1);
    map.insert("world", 2);
    map.insert("foo", 3);
    map.insert("hello", 4);

    std::cout << "hello : " << map.get("hello") << '\n'; // 4
    std::cout << "world : " << map.get("world") << '\n'; // 2
    std::cout << "getLast : " << map.getLast() << '\n'; // hello
    std::cout << "getFirst : " << map.getEarliest() << '\n'; // world

    map.remove("world");
    std::cout << "after remove(world), getFirst : " << map.getEarliest() << '\n'; // foo
}