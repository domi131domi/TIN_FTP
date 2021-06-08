#include "FTPServer.h"

int main(int argc, char const *argv[])
{
    if(argc != 3)
        std::cout << "Program needs exacly 2 arguments: port and path" << std::endl;
    int port;
    try
    {
        port = std::stoi(argv[1]);
    }
    catch(...)
    {
        std::cout<<"Bad port number"<<std::endl;
        return 0;
    }
    FTPServer server;
    server.Start(port, argv[2]);
    server.Stop();
    return 0;
}
