#include "ClientPIModule.h"
#define REPLYSIZE 1024

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

bool ClientPIModule::SendCommand(std::string msg)
{
    if(!isRunning)
        return false;
    int val = send(client_socket, msg.c_str(), msg.length() * sizeof(char), 0);
    if(val < 0)
    {
        std::cout << "Connection lost" << std::endl;
        close(client_socket);
        return false;
    }

    char Replymsg[REPLYSIZE];
    val = read(client_socket, Replymsg, REPLYSIZE);
    std::string str(Replymsg);
    str = str.substr(0,val);
    if(val <= 0)
    {
        std::cout << "Connection lost" << std::endl;
        return false;
    }
    ManageReply(str);
    return true;
}

void ClientPIModule::Close()
{
    close(client_socket);
}

void ClientPIModule::ManageReply(std::string reply)
{
    if(reply.find("setPath ") == 0)
    {
        current_path = "~/" + reply.substr(8, reply.length());
    }
    else if(reply.find("error ") == 0)
    {
        std::cout << reply.substr(6, reply.length()) << std::endl;
    }
    else if(reply.find("show ") == 0)
    {
        std::cout << reply.substr(5, reply.length()) << std::endl;
    }
}

std::string ClientPIModule::GetPath()
{
    return current_path;
}
