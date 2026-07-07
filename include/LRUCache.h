#pragma once
#include "HashMap.h"
#include <list>
#include <optional>
#include <chrono>

template<typename KEY, typename VAL>
class LRUCache {
private:
    size_t capacity;
    std::list<KEY> order;
    HashMap<KEY, VAL> values;
    HashMap<KEY, typename std::list<KEY>::iterator> iterators;

public:
    LRUCache(size_t capacity) : capacity(capacity), values(capacity * 2), iterators(capacity * 2) {}

    std::optional<VAL> get(const KEY& key) {
        auto val = values.get(key);
        if (!val) {
            auto it = iterators.get(key);
            if(it) {
                order.erase(*it);
                iterators.del(key);
            }
            return std::nullopt;
        }
        auto it = iterators.get(key);
        order.erase(*it);
        order.push_front(key);
        iterators.set(key, order.begin());
        return val;
    }

    void set(const KEY& key, const VAL& value, int ttl = 0) {
        if (values.get(key)) {
            values.set(key, value, ttl);
            auto it = iterators.get(key);
            order.erase(*it);
            order.push_front(key);
            iterators.set(key, order.begin());
            return;
        }
        if (order.size() >= capacity) {
            KEY lru_key = order.back();
            order.pop_back();
            values.del(lru_key);
            iterators.del(lru_key);
        }
        values.set(key, value, ttl);
        order.push_front(key);
        iterators.set(key, order.begin());
    }

    bool del(const KEY& key) {
        if (!values.get(key)) return false;
        auto it = iterators.get(key);
        order.erase(*it);
        values.del(key);
        iterators.del(key);
        return true;
    }
};