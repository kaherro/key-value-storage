#include "LRUCache.h"
#include <cassert>
#include <iostream>
#include <thread>

void test_basic() {
    std::cout << "Running basic tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("foo", 1);
    assert(cache.get("foo") == 1);
    assert(cache.get("bar") == std::nullopt);
    std::cout << "basic tests passed\n";
}

void test_overwrite() {
    std::cout << "Running overwrite tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("foo", 1);
    cache.set("foo", 2);
    assert(cache.get("foo") == 2);
    std::cout << "overwrite tests passed\n";
}

void test_del() {
    std::cout << "Running del tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("foo", 1);
    assert(cache.del("foo") == true);
    assert(cache.get("foo") == std::nullopt);
    assert(cache.del("foo") == false);
    std::cout << "del tests passed\n";
}

void test_eviction_on_capacity() {
    std::cout << "Running eviction tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    cache.set("d", 4);
    assert(cache.get("a") == std::nullopt);
    assert(cache.get("b") == 2);
    assert(cache.get("c") == 3);
    assert(cache.get("d") == 4);
    std::cout << "eviction tests passed\n";
}

void test_get_updates_recency() {
    std::cout << "Running recency tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    assert(cache.get("a") == 1);
    cache.set("d", 4);
    assert(cache.get("b") == std::nullopt);
    assert(cache.get("a") == 1);
    assert(cache.get("c") == 3);
    assert(cache.get("d") == 4);
    std::cout << "recency tests passed\n";
}

void test_set_existing_key_updates_recency() {
    std::cout << "Running set-existing-key recency tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    cache.set("a", 100);
    cache.set("d", 4);
    assert(cache.get("b") == std::nullopt);
    assert(cache.get("a") == 100);
    assert(cache.get("c") == 3);
    assert(cache.get("d") == 4);
    std::cout << "set-existing-key recency tests passed\n";
}

void test_keys_order() {
    std::cout << "Running keys() order tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    auto keys = cache.keys();
    assert(keys.size() == 3);
    assert(keys[0] == "c");
    assert(keys[1] == "b");
    assert(keys[2] == "a");
    std::cout << "keys() order tests passed\n";
}

void test_ttl() {
    std::cout << "Running ttl tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("foo", 1, 1);
    assert(cache.get("foo") == 1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(cache.get("foo") == std::nullopt);
    cache.set("foo", 2, 1);
    assert(cache.get("foo") == 2);
    std::cout << "ttl tests passed\n";
}

void test_get_val_without_update() {
    std::cout << "Running get_val_without_update tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    assert(cache.get_val_without_update("a") == 1);
    cache.set("d", 4);
    assert(cache.get("a") == std::nullopt);
    assert(cache.get("b") == 2);
    assert(cache.get("c") == 3);
    assert(cache.get("d") == 4);
    std::cout << "get_val_without_update tests passed\n";
}

void test_get_expiration_without_update() {
    std::cout << "Running get_expiration_without_update tests...\n";
    LRUCache<std::string, int> cache(3);
    cache.set("foo", 1);
    cache.set("bar", 2, 100);
    assert(cache.get_expiration_without_update("foo") == 0);
    assert(cache.get_expiration_without_update("bar") > 0);
    assert(cache.get_expiration_without_update("missing") == std::nullopt);
    std::cout << "get_expiration_without_update tests passed\n";
}

int main() {
    std::cout << "==========================\n";
    std::cout << " STARTING LRUCACHE TESTS...\n";
    std::cout << "==========================\n";

    test_basic();
    test_overwrite();
    test_del();
    test_eviction_on_capacity();
    test_get_updates_recency();
    test_set_existing_key_updates_recency();
    test_keys_order();
    test_ttl();
    test_get_val_without_update();
    test_get_expiration_without_update();

    std::cout << "==========================\n";
    std::cout << "   LRUCACHE TESTS PASSED! \n";
    std::cout << "==========================\n";
}