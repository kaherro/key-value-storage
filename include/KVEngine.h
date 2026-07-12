#pragma once
#include "LRUCache.h"
#include "WAL.h"
#include <string>
#include <optional>

class KVEngine {
private: 
    LRUCache<std::string, std::string> cache;
    WAL wal; 
public:
    KVEngine(size_t max_size = 1000, const std::string &path = "kv.wal"); 
    void set(const std::string &key, const std::string &value, int ttl);
    std::optional<std::string> get(const std::string &key);
    bool del(const std::string &key); 
};
