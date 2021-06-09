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
    else if(reply.find("send password ") == 0)
    {
        //std::cout << "oooj bedzie logowane " << std::endl;
        if(isLogged)
            std::cout << "You are already logged in." << std::endl;
        else
        {
            std::string username = reply.substr(14, reply.length());
            std::cout << "password: ";
            std::string typedPassword;
            getline(std::cin, typedPassword);
            SendCommand("log me " + username + "\n" + typedPassword);
        }
    }
    else if(reply.find("login result ") == 0)
    {
        std::string answer = reply.substr(13, reply.length());
        if(answer == "You have been logged in successfully!")
            isLogged = true;
        std::cout << answer << std::endl;
    }
    else if(reply.find("logout result ") == 0)
    {
        isLogged = false;
        std::cout << reply.substr(14, reply.length()) << std::endl;
    }
    else if(reply == "signin")
    {
        std::string username;
        std::string password;
        std::cout << "username: ";
        getline(std::cin, username);
        std::cout << "password: ";
        getline(std::cin, password);
        SendCommand("new account " + username + "\n" + password);
    }

}

std::string ClientPIModule::GetPath()
{
    return current_path;
}
