#pragma once
#include <iostream>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <fstream>

#define SERVER_IP "127.0.0.1"

class ClientDTPModule
{
private:
  const int SERVER_PORT;
public:
  ClientDTPModule(const int port) : SERVER_PORT(port){}
  void proceedSend(uint16_t port, const std::string& fileName);
  void handleReceive(uint16_t port, const std::string& fileName);
  int createClientSocket(uint16_t port);
};
