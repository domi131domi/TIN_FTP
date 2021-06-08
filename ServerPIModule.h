#pragma once
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <thread>
#include <atomic>
#include <poll.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "ClientInfo.h"
#include "ServerDTPModule.h"

class ServerPIModule
{
private:
	int server_fd;
	std::atomic<bool> isRunning{false};
	struct sockaddr_in address;
	std::vector<std::thread> threads;
	ServerDTPModule* serverDtpModule;
	std::map<int,ClientInfo>* clients;
public:
	bool Start();
	void Stop();
	bool SetSocket(int port);
	void SetClients(std::map<int,ClientInfo>* clients);
	void ConnectWithDtp(ServerDTPModule* adress);
private:
	void ManageNewClient(int new_socket);
	void ListenForConnections();
	bool Read(int socket);
	void ManageCommand(int socket,std::string command);


};
