#include "FTP.h"
#include <iomanip>
#include <openssl/sha.h>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cmath>

using namespace std;

int FTP::sendBuffer(int sock_fd, const char* buffer, int bufferSize, int chunkSize) {
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

long FTP::sendFile(int sock_fd, const string &fileName, int chunkSize) {
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
    }
    i -= l;
  }

  delete[] buffer;

  file.close();
  return isError ? FILE_SEND_ERR : fileSize;
}

int FTP::receiveBuffer(int sock_fd, char* buffer, int bufferSize, int chunkSize) {
    int i = 0;
    while (i < bufferSize) {
        const int l = recv(sock_fd, &buffer[i], min(chunkSize, bufferSize - i), 0);
        if (l < 0) {
            return -1;
        }
        i += l;
    }

    return i;
}

long FTP::receiveFile(int sock_fd, const string& fileName, int chunkSize) {
    ofstream file(fileName, ofstream::out);
    if (file.fail()) {
        return FILE_OPEN_ERR;
    }

    long fileSize;
    if (receiveBuffer(sock_fd,
                      reinterpret_cast<char*>(&fileSize),
                      sizeof(fileSize)) != sizeof(fileSize)) {
        return FILE_LENGTH_RECV_ERR;
    }

    char* buffer = new char[chunkSize];
    bool isError = false;
    long i = fileSize;
    while (i != 0) {
        const int r = receiveBuffer(sock_fd, buffer, (int) min(i, (long) chunkSize));
        if ((r < 0) || !file.write(buffer, r)) {
            isError = true;
            break;
        }
        i -= r;
    }
    delete[] buffer;

    file.close();

    return isError ? FILE_RECEIVE_ERR : fileSize;
}

bool FTP::Send(const int socket, std::string message){
    int val = send(socket, message.c_str(), message.length() * sizeof(char), 0);
    if(val < 0)
    {
        std::cout << "Connection lost" << std::endl;
        close(socket);
        return false;
    }

    return true;
}

std::string FTP::Read(const int socket){
    char Replymsg[REPLYSIZE];
    int val = read(socket, Replymsg, REPLYSIZE);
    if(val < 0)
    {
        std::cout << "Connection lost" << std::endl;
        return "";
    }

    std::string str(Replymsg);
    str = str.substr(0,val);

    return str;
}

std::string FTP::SHA256(const char* const filepath)
{
  std::ifstream file(filepath, std::ios::in | std::ios::binary);

  if (!file.good()) {
    throw std::runtime_error("Problem podczas otwierania pliku do obliczania sumy kontrolnej");
  }

  constexpr const std::size_t buffer_size { 1 << 12 }; //2kb
  char buffer[buffer_size];

  unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

  SHA256_CTX ctx;
  SHA256_Init(&ctx);

  while (file.good()) {
    file.read(buffer, buffer_size);
    SHA256_Update(&ctx, buffer, file.gcount());
  }

  SHA256_Final(hash, &ctx);
  file.close();

  std::ostringstream os;
  os << std::hex << std::setfill('0');

  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    os << std::setw(2) << static_cast<unsigned int>(hash[i]);
  }

  return os.str();
}

long FTP::getFileSize(const string& fileName) {
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
