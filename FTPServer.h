#pragma once
#include "ServerPIModule.h"
#include "ServerDTPModule.h"

class FTPServer
{
private:
    ServerPIModule serverPiModule;
    ServerDTPModule serverDTPModule;
    std::map<int,ClientInfo> clients;

public:
    bool Start(int port, std::string path);
    void Stop();
};
