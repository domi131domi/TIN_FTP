#include "ClientPIModule.h"

ClientPIModule::ClientPIModule(int port, std::string ip_str)
{
    struct sockaddr_in serv_addr;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip_str.c_str(), &serv_addr.sin_addr)<=0)
    {
        perror("Invalid adress");
        return;
    }

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return;
    }

    isRunning = true;
}

bool ClientPIModule::Send(std::string msg)
{
    if(!isRunning)
        return false;
    int val = send(client_socket, msg.c_str(), msg.length() * sizeof(char), 0);
    if(val < 0)
    {
        perror("Connection lost");
        return false;
    }
    return true;
}
