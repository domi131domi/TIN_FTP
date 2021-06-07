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
#include <arpa/inet.h>
#include "Command.h"

class ClientPIModule
{
private:
	int client_socket;
    bool isRunning = false;
public:
	void Stop();
	ClientPIModule(int port, std::string ip_str);
	bool Send(std::string msg);

};
