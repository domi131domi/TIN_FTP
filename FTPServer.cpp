#include "FTPServer.h"

bool FTPServer::Start(int port, std::string path)
{
    if(!serverPiModule.SetSocket(port))
        return false;

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
