#include "KVEngine.h"

KVEngine::KVEngine(size_t max_size, const std::string &path) 
    : cache(max_size), wal(path, cache) {
    wal.replay(); 
}

void KVEngine::set(const std::string &key, const std::string &value, int ttl) {
    std::unique_lock lock(mutex1);
    cache.set(key, value, ttl); 
    wal.log_set(key, value, ttl); 
}

std::optional<std::string> KVEngine::get(const std::string &key) {
    std::shared_lock lock(mutex1);
    return cache.get(key); 
}

bool KVEngine::del(const std::string &key) {
    std::unique_lock lock(mutex1);
    bool result = cache.del(key); 
    if(result) wal.log_del(key); 
    return result; 
}