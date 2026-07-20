#include "Server.h"

int main() {
    KVEngine engine(100, "kv.wal");
    Server server(6380, engine);
    server.run();
    return 0;
}