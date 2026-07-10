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
    HashMap<KEY, long long> expirations; 

public:
    LRUCache(size_t capacity) : capacity(capacity), values(capacity * 2), iterators(capacity * 2), expirations(capacity * 2) {}

    std::optional<VAL> get(const KEY &key) {
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

    void set(const KEY &key, const VAL &value, int ttl = 0) {
        long long expires_at = 0;
        if(ttl > 0) {
            expires_at = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count() + ttl;
        }
        expirations.set(key, expires_at); 

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

    bool del(const KEY &key) {
        if (!values.get(key)) return false;
        auto it = iterators.get(key);
        order.erase(*it);
        values.del(key);
        iterators.del(key);
        expirations.del(key); 
        return true;
    }

    std::vector<KEY> keys() const {
        std::vector<KEY> res; 
        for(const auto &key : order) {
            res.push_back(key); 
        }
        return res; 
    }

    std::optional<VAL> get_val_without_update(const KEY &key) {
        return values.get(key);
    }

    std::optional<long long> get_expiration_without_update(const KEY &key) {
        return expirations.get(key);
    }
};