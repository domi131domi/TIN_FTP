#include "ClientDTPModule.h"

using std::cout;
using std::string;
using std::min;
using std::ifstream;
using std::endl;

void ClientDTPModule::proceedSend(uint16_t port) {
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

  long rc = sendFile(sock_fd, "tosend.txt");
  if (rc < 0) {
    cout << "Failed to send file: " << rc << endl;
  }

  cout << "done";
  shutdown(sock_fd, SHUT_RDWR);
  close(sock_fd);
}

long ClientDTPModule::getFileSize(const string& fileName) {
  FILE* pFile;
  long size = -1;

  pFile = fopen(fileName.c_str(), "rb");
  if (pFile != nullptr) {
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    fclose(pFile);
  } else {
    perror ("Error opening file");
  }

  return size;
}

int ClientDTPModule::sendBuffer(int sock_fd, const char* buffer, int bufferSize, int chunkSize) {
  int i = 0;
  while (i < bufferSize) {
    const int l = send(sock_fd, &buffer[i], min(chunkSize, bufferSize - i), 0);
    if (l < 0) {
      return -1;
    }
    i += l;
  }
  return i;
}

long ClientDTPModule::sendFile(int sock_fd, const string &fileName, int chunkSize) {
  const long fileSize = getFileSize(fileName);
  if (fileSize < 0){
    return FILE_OPEN_ERR;
  }

  ifstream file(fileName, ifstream::in);
  if (file.fail()) {
    return FILE_OPEN_ERR;
  }

  int sentBytes = sendBuffer(sock_fd, reinterpret_cast<const char *>(&fileSize), sizeof(fileSize));
  if (sentBytes != sizeof(fileSize)) {
    return FILE_LENGTH_READ_ERR;
  }

  char* buffer = new char[chunkSize];
  bool isError = false;
  long i = fileSize;
  while(i != 0) {
    const long ssize = min(i, (long) chunkSize);

    if (!file.read(buffer, ssize)) {
      isError = true;
      break;
    }

    long l = sendBuffer(sock_fd, buffer, (int) ssize);

    if (l < 0) {
      isError = true;
      break;
      i -= l;
    }
    delete[] buffer;

    file.close();
    return isError ? FILE_SEND_ERR : fileSize;
  }

  return 0;
}
