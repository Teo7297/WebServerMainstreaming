#include "Server.h"

int main(int argc, char* argv[])
{
    fs::path configPath(argv[1]);

    MC::Server server(configPath);
    server.CreateHTTPServer();

    return 0;
}