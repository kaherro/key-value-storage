#pragma once
#include <string>
#include <vector>
#include <optional>

template<typename KEY, typename VAL>
class HashMap {
private:
    struct cell {
        KEY key; 
        VAL value; 
        bool used = false; 
        bool deleted = false; 
    };
    
    std::vector<cell> cells; 
    size_t size = 0; 
    size_t capacity = 0; 
    const size_t MOD = 1e9 + 7; 

    size_t hash(const KEY &key, size_t capacity) const {
        std::string s; 
        if constexpr (std::is_same_v<KEY, std::string>) {
            s = key; 
        } else {
            s = std::to_string(key); 
        }
        size_t h = 0; 
        for(char c : s) {
            h = (h * 31 + c) % MOD; 
        }
        return h % capacity; 
    }
public:
    HashMap(size_t capacity = 16) {
        this->capacity = capacity; 
        cells.resize(capacity); 
    }

    void rehash() {
        size_t capacity_new = capacity * 2; 
        std::vector<cell> cells_new(capacity_new);
        for(auto &c : cells) {
            if(c.used && !c.deleted) {
                size_t i = hash(c.key, capacity_new); 
                while(cells_new[i].used) {
                    i = (i + 1) % capacity_new; 
                }
                cells_new[i] = {c.key, c.value, true, false}; 
            }
        }
        cells = std::move(cells_new);
        capacity = capacity_new;
    }

    void set(const KEY &key, const VAL &value) {
        if (size * 4 >= capacity * 3) rehash(); 
        auto i = hash(key, capacity); 
        while(cells[i].used && !cells[i].deleted && cells[i].key != key) {
            i = (i + 1) % capacity; 
        }
        if (!cells[i].used) size++; 
        cells[i] = {key, value, true, false};
    }

    std::optional<VAL> get(const KEY &key) const {
        auto i = hash(key, capacity); 
        int ops = 0; 
        while(ops < capacity) {
            if (!cells[i].used && !cells[i].deleted) {
                return std::nullopt;
            }
            if(cells[i].key == key && !cells[i].deleted) {
                return cells[i].value; 
            }
            i = (i + 1) % capacity; 
            ops++; 
        }
        return std::nullopt; 
    };

    bool del(const KEY &key) {
        auto i = hash(key, capacity); 
        int ops = 0; 
        while(ops < capacity) {
            if (!cells[i].used && !cells[i].deleted) {
                return false;
            }
            if(cells[i].key == key && !cells[i].deleted) {
                cells[i].deleted = true; 
                size--; 
                return true; 
            }
            i = (i + 1) % capacity; 
            ops++; 
        }
        return false; 
    };
};