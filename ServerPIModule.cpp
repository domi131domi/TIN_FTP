#include "ServerPIModule.h"
#include <fstream>
#include "FTP.h"
#define COMMANDSIZE 1024
#define TIMEOUT 2
#define MAX_CLIENTS 3

bool ServerPIModule::Start()
{
    if(!isRunning)
    {
        perror("Cannot start server");
        return false;
    }
    threads.push_back(std::thread(&ServerPIModule::ListenForConnections, this));
    std::cout << "Type 'stop' to stop server" << std::endl;
    std::string t;
    while(t != "stop")
    {
        getline(std::cin, t);
        if(t.find("register ") == 0)
            RegisterUsername(t.substr(9, t.length()));

    }
    return true;
}

bool ServerPIModule::Read(int socket)
{
    std::string reply = FTP::Read(socket);
    if( reply.length() == 0 ){
        std::cout << "User nr: " << socket << " disconnected" << std::endl;
        return false;
    }

    std::cout << "Got command from user nr: " << socket << " " << reply << std::endl;
    ManageCommand(socket,reply);
    return true;
}

bool ServerPIModule::SetSocket(int port)
{
    if(isRunning)
        return false;

    int opt = 1;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return false;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setting options error");
        return false;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("Binding error");
        return false;
    }
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen error");
        return false;
    }
    isRunning = true;

    return true;
}

void ServerPIModule::ListenForConnections()
{
    std::cout << "Listen for connections start" << std::endl;
    struct pollfd fds;
    fds.fd = server_fd;
    fds.events = POLLIN;
    while(isRunning)
    {
        int new_socket;
        int addrlen = sizeof(address);

        int p = poll(&fds, 1, TIMEOUT*1000);
        if(p == 0)
            continue;
        else if(p < 0)
            perror("poll error");

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen)) < 0)
        {
            perror("Accept error");
        }
        threads.push_back(std::thread(&ServerPIModule::ManageNewClient,this, new_socket));
    }
    std::cout << "Listen for connections stop" << std::endl;
}

void ServerPIModule::ManageNewClient(int new_socket)
{
    std::cout << "New Client Connected " << new_socket << std::endl;
    if(clients == nullptr)
        return;
    ClientInfo info;
    (*clients)[new_socket] = info;
    struct pollfd fds;
    fds.fd = new_socket;
    fds.events = POLLIN;

    while(isRunning)
    {
        int p = poll(&fds, 1, TIMEOUT*1000);
        if(p == 0)
            continue;
        else if(p < 0)
        {
            perror("poll error");
            close(new_socket);
        }
        if(!Read(new_socket))
            break;
    }
}

void ServerPIModule::Stop()
{
    if(!isRunning)
        return;
    std::cout << "Start closing server" << std::endl;
    isRunning = false;
    for (auto& th : threads)
        th.join();
    close(server_fd);
}

void ServerPIModule::ManageCommand(int socket,std::string command)
{
    std::cout << "Sending command to DTPModule" << std::endl;
    std::string reply = "";
    if(serverDtpModule != nullptr)
    {
        reply = serverDtpModule->ManageCommand(command, &(*clients)[socket]);
    }
    FTP::Send(socket, reply.c_str());
}

void ServerPIModule::RegisterUsername(std::string username)
{
    std::string password;
    std::string fileContent = "";
    std::fstream registerFile;
    registerFile.open(serverDtpModule->getHomePath() + "RegistrationRequests.txt", std::fstream::in);
    std::string tmp;
    while(registerFile >> tmp)
    {
        if(tmp == username)
            registerFile >> password;
        else
        {
            fileContent += tmp + " ";
            registerFile >> tmp;    // password
            fileContent += tmp + "\n";
        }
    }
    registerFile.close();
    registerFile.open(serverDtpModule->getHomePath() + "RegistrationRequests.txt", std::fstream::out | std::fstream::trunc);
    registerFile << fileContent;
    registerFile.close();
    registerFile.open(serverDtpModule->getHomePath() + "ClientsData.txt", std::fstream::app);
    registerFile << username << " " << password << "\n";
    //std::string answer = "show Admin has accepted your request, " + username + "! Now you can log in using \'login\' <username> command.";
    //send(socket, answer.c_str(), answer.length() * sizeof(char), 0);
}

void ServerPIModule::ConnectWithDtp(ServerDTPModule* adress)
{
    serverDtpModule = adress;
}

void ServerPIModule::SetClients(std::map<int,ClientInfo>* clients)
{
    this->clients = clients;
}
