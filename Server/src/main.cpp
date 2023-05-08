#include <iostream>
#include "Server/server.h"

int main() {
    Server server(8080);
    server.Run();

    return 0;
}
