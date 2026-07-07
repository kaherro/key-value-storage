#include "HashMap.h"
#include <cassert>
#include <iostream>
#include <thread>

void test_basic() {
    HashMap<std::string, int> map;
    map.set("foo", 1);
    assert(map.get("foo") == 1);
    assert(map.get("bar") == std::nullopt);
    std::cout << "basic tests passed\n";
}

void test_overwrite() {
    HashMap<std::string, int> map;
    map.set("foo", 1);
    map.set("foo", 2);  
    assert(map.get("foo") == 2);
    std::cout << "overwrite tests passed\n";
}

void test_del() {
    HashMap<std::string, int> map;
    map.set("foo", 1);
    map.del("foo");
    assert(map.get("foo") == std::nullopt);
    assert(map.del("foo") == false); 
    std::cout << "del tests passed\n";
}

void test_tombstone() {
    HashMap<std::string, int> map;
    map.set("foo", 1);
    map.del("foo");
    map.set("foo", 2);
    assert(map.get("foo") == 2);
    std::cout << "tombstone tests passed\n";
}

void test_rehash() {
    HashMap<std::string, int> map(4); 
    for (int i = 0; i < 20; i++)
        map.set("key" + std::to_string(i), i);
    for (int i = 0; i < 20; i++)
        assert(map.get("key" + std::to_string(i)) == i);
    std::cout << "rehash tests passed\n";
}

void test_types() {
    HashMap<int, std::string> map;
    map.set(1, "one");
    map.set(2, "two");
    assert(map.get(1) == "one");
    assert(map.get(2) == "two");
    std::cout << "types tests passed\n";
}

void test_ttl() {
    HashMap<std::string, int> map;
    map.set("foo", 1, 1);
    assert(map.get("foo") == 1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(map.get("foo") == std::nullopt);
    map.set("foo", 2, 1); 
    assert(map.get("foo") == 2);
    std::cout << "ttl tests passed\n";
}

int main() {
    test_basic();
    test_overwrite();
    test_del();
    test_tombstone();
    test_rehash();
    test_types();
    test_ttl(); 
    std::cout << "All tests passed!\n";
}