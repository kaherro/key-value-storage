#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <thread>

Server::Server(int port, KVEngine &engine) : port(port), engine(engine) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    std::cout << "Server listening port " << port << "\n";
}

Server::~Server() {
    close(server_fd);
}

void Server::run() {
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;
        std::thread([this, client_fd]() {
            char buf[1024] = {};
            read(client_fd, buf, sizeof(buf));
            std::string response = handle_operation(buf);
            write(client_fd, response.c_str(), response.size());
            close(client_fd);
        }).detach();
    }
}

std::string Server::handle_operation(const std::string &cmd) {
    std::stringstream ss(cmd);
    std::string op;
    ss >> op;
    if (op == "SET") {
        std::string key, value;
        int ttl = 0;
        ss >> key >> value >> ttl;
        engine.set(key, value, ttl);
        return "OK\n";
    }
    if (op == "GET") {
        std::string key;
        ss >> key;
        auto val = engine.get(key);
        return val ? *val + "\n" : "(Key not found)\n";
    }
    if (op == "DEL") {
        std::string key;
        ss >> key;
        return engine.del(key) ? "OK\n" : "(Key not found)\n";
    }
    if (op == "EXISTS") {
        std::string key;
        ss >> key;
        return engine.get(key) ? "1\n" : "0\n";
    }
    return "Error: unknown command\n";
}
