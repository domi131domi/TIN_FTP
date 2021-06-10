#include "FTP.h"
#include <iomanip>
#include <openssl/sha.h>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

bool FTP::Send(const int socket, std::string message){
    int val = send(socket, message.c_str(), message.length() * sizeof(char), 0);
    std::cout<<val<<"\n";
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
    std::cout<<val<<"\n";
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