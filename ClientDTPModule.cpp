#include "ClientDTPModule.h"
#include "FTP.h"

using std::cout;
using std::string;
using std::min;
using std::ofstream;
using std::ifstream;
using std::endl;


void ClientDTPModule::proceedSend(uint16_t port, const std::string& fileName) {
  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = port};
  if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
  {
    perror("DTP failed in inet_pton()");
  }

  int sock_fd;
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("DTP failed in socket()");
  }

  socklen_t len = sizeof(serv_addr);

  if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connection failed");
  }

  long rc = FTP::sendFile(sock_fd, fileName);
  if (rc < 0) {
    cout << "Failed to send file: " << rc << endl;
  }

  cout << "done";
  shutdown(sock_fd, SHUT_RDWR);
  close(sock_fd);
}

void ClientDTPModule::handleReceive(uint16_t port, const std::string& fileName)
{
  int sock_fd = createClientSocket(port);

  long rc = FTP::receiveFile(sock_fd, fileName);
  if (rc < 0) {
    cout << "Failed to receive file: " << rc << endl;
  }
  else {
    cout << "File receive success" << endl;
  }

  close(sock_fd);
}

int ClientDTPModule::createClientSocket(uint16_t port) {

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = port};
  if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
  {
    perror("DTP failed in inet_pton()");
  }

  int sock_fd;
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("DTP failed in socket()");
  }

  socklen_t len = sizeof(serv_addr);

  if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connection failed");
  }

  return sock_fd;
}
