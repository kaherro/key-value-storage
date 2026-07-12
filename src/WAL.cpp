#include "WAL.h"
#include <sstream>
#include <iostream>

WAL::WAL(const std::string &path, LRUCache<std::string, std::string> &cache) 
    : path(path), cache(cache) {
    log.open(path, std::ios::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open WAL: " << path << "\n";
    }
}

void WAL::log_set(const std::string &key, const std::string &value, int ttl) {
    long long expires_at = 0;
    if (ttl > 0) {
        expires_at = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() + ttl;
    }
    log << "SET " << key << " " << value << " " << expires_at << std::endl;
    if(ops_count++ > snapshot_every) snapshot(); 
}

void WAL::log_del(const std::string &key) {
    log << "DEL " << key << std::endl; 
    if(ops_count++ > snapshot_every) snapshot(); 
}

void WAL::parse(const std::string &line) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;
    if (cmd == "SET") {
        std::string key, value;
        long long expires_at;
        ss >> key >> value >> expires_at;
        int remaining_ttl = 0;
        if (expires_at > 0) {
            long long now = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
            remaining_ttl = expires_at - now;
            if (remaining_ttl <= 0) return; 
        }
        cache.set(key, value, remaining_ttl);
    } 
    else if (cmd == "DEL") {
        std::string key;
        ss >> key;
        cache.del(key);
    }
}

void WAL::replay() {
    std::string snapshot_path = path + ".snapshot"; 
    std::ifstream snap(snapshot_path); 
    if(snap.is_open()) {
        std::string line;
        while(std::getline(snap, line)) {
            parse(line);
        }
    }
    std::ifstream wal(path);
    if (wal.is_open()) {
        std::string line;
        while(std::getline(wal, line)) {
            parse(line);
        }
    }
}

void WAL::snapshot() {
    std::string snapshot_path = path + ".snapshot";
    std::ofstream snap(snapshot_path); 
    for(const auto &key : cache.keys()) {
        auto val = cache.get_val_without_update(key); 
        long long expires_at = cache.get_expiration_without_update(key).value_or(0LL);
        if(val) {
            snap << "SET " << key << " " << *val << " " << expires_at << std::endl;
        }
    }
    log.close();
    log.open(path, std::ios::trunc);
    ops_count = 0; 
}
