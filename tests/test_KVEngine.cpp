#include "KVEngine.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdio>

void cleanup(const std::string &path) {
    std::remove(path.c_str());
    std::remove((path + ".snapshot").c_str());
}

void test_basic() {
    std::cout << "Running basic tests...\n";
    std::string path = "test_kv_basic.wal";
    cleanup(path);

    KVEngine engine(100, path);
    engine.set("foo", "bar");
    assert(engine.get("foo") == "bar");
    assert(engine.get("missing") == std::nullopt);

    cleanup(path);
    std::cout << "basic tests passed\n";
}

void test_overwrite() {
    std::cout << "Running overwrite tests...\n";
    std::string path = "test_kv_overwrite.wal";
    cleanup(path);

    KVEngine engine(100, path);
    engine.set("foo", "v1");
    engine.set("foo", "v2");
    assert(engine.get("foo") == "v2");

    cleanup(path);
    std::cout << "overwrite tests passed\n";
}

void test_del() {
    std::cout << "Running del tests...\n";
    std::string path = "test_kv_del.wal";
    cleanup(path);

    KVEngine engine(100, path);
    engine.set("foo", "bar");
    assert(engine.del("foo") == true);
    assert(engine.get("foo") == std::nullopt);
    assert(engine.del("foo") == false); 

    cleanup(path);
    std::cout << "del tests passed\n";
}

void test_ttl() {
    std::cout << "Running ttl tests...\n";
    std::string path = "test_kv_ttl.wal";
    cleanup(path);

    KVEngine engine(100, path);
    engine.set("foo", "bar", 1); 
    assert(engine.get("foo") == "bar");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(engine.get("foo") == std::nullopt);

    cleanup(path);
    std::cout << "ttl tests passed\n";
}

void test_persistence_across_restart() {
    std::cout << "Running persistence tests...\n";
    std::string path = "test_kv_persist.wal";
    cleanup(path);

    {
        KVEngine engine(100, path);
        engine.set("foo", "bar");
        engine.set("baz", "qux");
        engine.del("baz");
    }

    KVEngine engine2(100, path);
    assert(engine2.get("foo") == "bar");
    assert(engine2.get("baz") == std::nullopt); 

    cleanup(path);
    std::cout << "persistence tests passed\n";
}

void test_concurrent_distinct_keys() {
    std::cout << "Running concurrent distinct-keys tests...\n";
    std::string path = "test_kv_concurrent_distinct.wal";
    cleanup(path);

    KVEngine engine(1000, path);
    const int num_threads = 8;
    const int keys_per_thread = 50;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([&engine, t]() {
            for (int i = 0; i < keys_per_thread; i++) {
                std::string key = "t" + std::to_string(t) + "_k" + std::to_string(i);
                engine.set(key, "val" + std::to_string(i));
            }
        });
    }
    for (auto &th : threads) th.join();

    for (int t = 0; t < num_threads; t++) {
        for (int i = 0; i < keys_per_thread; i++) {
            std::string key = "t" + std::to_string(t) + "_k" + std::to_string(i);
            assert(engine.get(key) == "val" + std::to_string(i));
        }
    }

    cleanup(path);
    std::cout << "concurrent distinct-keys tests passed\n";
}

void test_concurrent_same_key_no_corruption() {
    std::cout << "Running concurrent same-key tests...\n";
    std::string path = "test_kv_concurrent_same.wal";
    cleanup(path);

    KVEngine engine(100, path);
    const int num_threads = 8;
    const int writes_per_thread = 100;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([&engine, t]() {
            for (int i = 0; i < writes_per_thread; i++) {
                engine.set("shared_key", "thread" + std::to_string(t));
            }
        });
    }
    for (auto &th : threads) th.join();

    auto val = engine.get("shared_key");
    assert(val.has_value());
    bool valid = false;
    for (int t = 0; t < num_threads; t++) {
        if (*val == "thread" + std::to_string(t)) valid = true;
    }
    assert(valid);

    cleanup(path);
    std::cout << "concurrent same-key tests passed\n";
}

int main() {
    std::cout << "==========================\n";
    std::cout << " STARTING KVENGINE TESTS...\n";
    std::cout << "==========================\n";

    test_basic();
    test_overwrite();
    test_del();
    test_ttl();
    test_persistence_across_restart();
    test_concurrent_distinct_keys();
    test_concurrent_same_key_no_corruption();

    std::cout << "==========================\n";
    std::cout << "   KVENGINE TESTS PASSED! \n";
    std::cout << "==========================\n";
}