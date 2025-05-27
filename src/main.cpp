#include "Server.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Please provide a config file path as argument" << std::endl;
        return -1;
    }
    fs::path configPath(argv[1]);

    MC::Server server(configPath);

    if(server.IsConfigLoaded())
        server.CreateServer();

    return 0;
}