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
#define FILE_OPEN_ERR -1
#define FILE_LENGTH_RECV_ERR -2
#define FILE_RECEIVE_ERR -3
#define FILE_LENGTH_READ_ERR -4
#define FILE_SEND_ERR -5

class ClientDTPModule
{
private:
  const int SERVER_PORT;
public:
  ClientDTPModule(const int port) : SERVER_PORT(port){}
  void proceedSend(uint16_t port, const std::string& fileName);
  long getFileSize(const std::string& fileName);
  int sendBuffer(int sock_fd, const char* buffer, int buffereSize, int chunkSize = 4 * 1024);
  long sendFile(int sock_fd, const std::string& fileName, int chunkSize = 64 * 1024);
  void handleReceive(uint16_t port, const std::string& fileName);
  int createClientSocket(uint16_t port);
  int receiveBuffer(int sock_fd, char* buffer, int bufferSize, int chunkSize = 4 * 1024);
  long receiveFile(int sock_fd, const std::string& fileName, int chunkSize = 64 * 1024);
};
