#include "WAL.h"
#include "LRUCache.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdio>

void cleanup(const std::string &path) {
    std::remove(path.c_str());
    std::remove((path + ".snapshot").c_str());
}

void test_basic_persist() {
    std::cout << "Running basic persist tests...\n";
    std::string path = "test_wal_basic.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        wal.replay(); 
        cache.set("foo", "bar");
        wal.log_set("foo", "bar", 0);
    } 

    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("foo") == "bar");

    cleanup(path);
    std::cout << "basic persist tests passed\n";
}

void test_del_persisted() {
    std::cout << "Running del persist tests...\n";
    std::string path = "test_wal_del.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        cache.set("foo", "bar");
        wal.log_set("foo", "bar", 0);
        cache.del("foo");
        wal.log_del("foo");
    }

    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("foo") == std::nullopt);

    cleanup(path);
    std::cout << "del persist tests passed\n";
}

void test_overwrite_persisted() {
    std::cout << "Running overwrite persist tests...\n";
    std::string path = "test_wal_overwrite.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        cache.set("foo", "v1");
        wal.log_set("foo", "v1", 0);
        cache.set("foo", "v2");
        wal.log_set("foo", "v2", 0);
    }

    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("foo") == "v2");

    cleanup(path);
    std::cout << "overwrite persist tests passed\n";
}

void test_multiple_keys_persisted() {
    std::cout << "Running multiple keys persist tests...\n";
    std::string path = "test_wal_multi.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        for (int i = 0; i < 10; i++) {
            std::string key = "key" + std::to_string(i);
            std::string val = "val" + std::to_string(i);
            cache.set(key, val);
            wal.log_set(key, val, 0);
        }
    }

    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    for (int i = 0; i < 10; i++) {
        std::string key = "key" + std::to_string(i);
        std::string val = "val" + std::to_string(i);
        assert(cache2.get(key) == val);
    }

    cleanup(path);
    std::cout << "multiple keys persist tests passed\n";
}

void test_ttl_expired_not_replayed() {
    std::cout << "Running ttl expired tests...\n";
    std::string path = "test_wal_ttl_expired.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        cache.set("foo", "bar", 1); 
        wal.log_set("foo", "bar", 1);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("foo") == std::nullopt);

    cleanup(path);
    std::cout << "ttl expired tests passed\n";
}

void test_ttl_valid_replayed() {
    std::cout << "Running ttl valid tests...\n";
    std::string path = "test_wal_ttl_valid.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(10);
        WAL wal(path, cache);
        cache.set("foo", "bar", 100); 
        wal.log_set("foo", "bar", 100);
    }

    LRUCache<std::string, std::string> cache2(10);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("foo") == "bar");

    cleanup(path);
    std::cout << "ttl valid tests passed\n";
}

void test_snapshot_created_after_threshold() {
    std::cout << "Running snapshot creation tests...\n";
    std::string path = "test_wal_snapshot.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(200);
        WAL wal(path, cache);
        for (int i = 0; i < 150; i++) {
            std::string key = "key" + std::to_string(i);
            cache.set(key, "val");
            wal.log_set(key, "val", 0);
        }
    }

    std::ifstream snap(path + ".snapshot");
    assert(snap.is_open());

    cleanup(path);
    std::cout << "snapshot creation tests passed\n";
}

void test_replay_from_snapshot_and_log() {
    std::cout << "Running replay-from-snapshot tests...\n";
    std::string path = "test_wal_snapshot_replay.log";
    cleanup(path);

    {
        LRUCache<std::string, std::string> cache(200);
        WAL wal(path, cache);
        for (int i = 0; i < 150; i++) {
            std::string key = "key" + std::to_string(i);
            cache.set(key, "old");
            wal.log_set(key, "old", 0);
        }
        cache.set("after_snapshot", "fresh");
        wal.log_set("after_snapshot", "fresh", 0);
    }

    LRUCache<std::string, std::string> cache2(200);
    WAL wal2(path, cache2);
    wal2.replay();
    assert(cache2.get("key0") == "old");
    assert(cache2.get("key149") == "old");
    assert(cache2.get("after_snapshot") == "fresh");

    cleanup(path);
    std::cout << "replay-from-snapshot tests passed\n";
}

int main() {
    std::cout << "==========================\n";
    std::cout << "   STARTING WAL TESTS...\n";
    std::cout << "==========================\n";

    test_basic_persist();
    test_del_persisted();
    test_overwrite_persisted();
    test_multiple_keys_persisted();
    test_ttl_expired_not_replayed();
    test_ttl_valid_replayed();
    test_snapshot_created_after_threshold();
    test_replay_from_snapshot_and_log();

    std::cout << "==========================\n";
    std::cout << "     WAL TESTS PASSED! \n";
    std::cout << "==========================\n";
}