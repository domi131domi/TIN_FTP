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
#include <filesystem>
#include "ClientDTPModule.h"
#include "FTP.h"
#define LOCALHOST "127.0.0.1"

class ClientPIModule
{
private:
	const std::string SEND_ACCEPT_RESPONSE = "send_accept";
	const std::string GET_ACCEPT_RESPONSE = "get_accept";

	int client_socket;
	bool isLogged = false;
	std::string current_path = "~/";
	ClientDTPModule* clientDTPModule;

	const std::string SEND_FILE_COMMAND = "send ";
	const std::string GET_FILE_COMMAND = "get ";

	std::string file_to_send;
	std::string file_to_receive;
public:
	bool isRunning = false;
	void Stop();
	ClientPIModule(int port, std::string ip_str);
	bool SendCommand(std::string msg);
	void ManageReply(std::string reply);
	void Close();
	std::string GetPath();
	void connectToDTP(ClientDTPModule* address);
	bool isFileAlreadyOnServer(std::string msg);
};
