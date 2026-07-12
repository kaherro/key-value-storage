#pragma once
#include "LRUCache.h"
#include <fstream>
#include <string>

class WAL {
private:
    std::ofstream log;
    std::string path;
    int ops_count = 0;
    const int snapshot_every = 100;
    LRUCache<std::string, std::string> &cache;
    void parse(const std::string &line);
    void snapshot();
public:
    WAL(const std::string &path, LRUCache<std::string, std::string> &cache);
    void log_set(const std::string &key, const std::string &value, int ttl);
    void log_del(const std::string &key);
    void replay();
};