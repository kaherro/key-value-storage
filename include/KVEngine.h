#pragma once
#include "LRUCache.h"
#include "WAL.h"
#include <string>
#include <optional>
#include <mutex>
#include <shared_mutex>

class KVEngine {
private: 
    LRUCache<std::string, std::string> cache;
    WAL wal; 
    mutable std::shared_mutex mutex1;
public:
    KVEngine(size_t max_size = 1000, const std::string &path = "kv.wal"); 
    void set(const std::string &key, const std::string &value, int ttl = 0);
    std::optional<std::string> get(const std::string &key);
    bool del(const std::string &key); 
};
