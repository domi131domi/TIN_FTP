#include "ServerPIModule.h"
#define COMMANDSIZE 1024
#define TIMEOUT 2

void ServerPIModule::Start()
{
    if(!isRunning)
    {
        perror("Cannot start server");
        return;
    }
    threads.push_back(std::thread(&ServerPIModule::ListenForConnections, this));
    std::cout << "Press enter to stop" << std::endl;
    std::string t;
    std::cin >> t;
}

bool ServerPIModule::Read(int socket)
{
    char msg[COMMANDSIZE];
    int val = read(socket, msg, COMMANDSIZE);
    std::string str(msg);
    str = str.substr(0,val);
    if(val <= 0)
    {
        std::cout << "Connection lost from client nr: " << socket << std::endl;
        return false;
    }
    std::cout << "Got message from user nr: " << socket << " " << str << std::endl;
    return true;
}

ServerPIModule::ServerPIModule(int port, std::string folderPath)
{
    int opt = 1;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setting options error");
        return;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("Binding error");
        return;
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen error");
        return;
    }
    isRunning = true;
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
