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
#include <iostream>
#include "ClientDTPModule.h"

class ClientPIModule
{
private:
	int client_socket;
	bool isLogged = false;
  bool isRunning = false;
	std::string current_path = "~/";
	ClientDTPModule* clientDTPModule;
public:
	void Stop();
	ClientPIModule(int port, std::string ip_str);
	bool SendCommand(std::string msg);
	void ManageReply(std::string reply);
	void Close();
	std::string GetPath();
	void connectToDTP(ClientDTPModule* address);
};
