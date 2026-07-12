#pragma once
#include "KVEngine.h"
#include <string>

class Server {
    int server_fd;
    KVEngine& engine;
    int port;
    std::string handle_operation(const std::string &cmd);
public:
    ~Server();
    Server(int port, KVEngine &engine);
    void run();
};