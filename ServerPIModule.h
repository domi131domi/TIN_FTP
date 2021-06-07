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
#include "Command.h"

class ServerPIModule
{
private:
	int server_fd;
	std::atomic<bool> isRunning{false};
	struct sockaddr_in address;
	std::vector<std::thread> threads;
public:
	void Start();
	void Stop();
	ServerPIModule(int port, std::string folderPath);
private:
	void ManageNewClient(int new_socket);
	void ListenForConnections();
	bool Read(int socket);

};
