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