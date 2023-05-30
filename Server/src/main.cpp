#include <iostream>
#include "Server/server.h"

int main() {
    try {
        Server server(13);
        server.run();
    } catch (std::exception &e) {
        std::cerr << e.what();
    }
    return 0;
}
