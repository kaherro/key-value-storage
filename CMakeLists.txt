cmake_minimum_required(VERSION 3.10)
project(key_value_storage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(Threads REQUIRED)

add_executable(kvserver
    main.cpp
    src/KVEngine.cpp
    src/WAL.cpp
    src/Server.cpp
)

target_link_libraries(kvserver PRIVATE Threads::Threads)

enable_testing()

add_executable(test_hashmap tests/test_HashMap.cpp)
target_link_libraries(test_hashmap PRIVATE Threads::Threads)
add_test(NAME HashMap_tests COMMAND test_hashmap)

add_executable(test_lrucache tests/test_LRUCache.cpp)
target_link_libraries(test_lrucache PRIVATE Threads::Threads)
add_test(NAME LRUCache_tests COMMAND test_lrucache)

add_executable(test_wal tests/test_WAL.cpp src/WAL.cpp)
target_link_libraries(test_wal PRIVATE Threads::Threads)
add_test(NAME wal_tests COMMAND test_wal)

add_executable(test_kvengine tests/test_KVEngine.cpp src/KVEngine.cpp src/WAL.cpp)
target_link_libraries(test_kvengine PRIVATE Threads::Threads)
add_test(NAME KVEngine_tests COMMAND test_kvengine)