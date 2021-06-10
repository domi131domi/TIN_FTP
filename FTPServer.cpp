#include "FTPServer.h"
#include <filesystem>

bool FTPServer::Start(int port, std::string path)
{
    if(!serverPiModule.SetSocket(port))
        return false;

    if( path.back() != '/' ){
        std::cout << "Podana sciezka musi byc zakonczona \"/\"\n";
        return false;
    }

    if(!std::filesystem::is_directory(path)){
        std::cout << "Podano bledna sciezke\n";
        return false;
    }

    serverDTPModule.SetPath(path);
    serverPiModule.ConnectWithDtp(&serverDTPModule);
    serverPiModule.SetClients(&clients);

    if(!serverPiModule.Start())
        return false;
    return true;
}

void FTPServer::Stop()
{
    serverPiModule.Stop();
}
