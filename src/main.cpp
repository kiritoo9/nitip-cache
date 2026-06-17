#include "server.h"

int main()
{
    NitipServer server;
    
    // server.cli();
    server.start(6379);

    return 0;
}