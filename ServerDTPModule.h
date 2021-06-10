#pragma once
#include <string>
#include <experimental/filesystem>
#include <iostream>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <filesystem>
#include "ClientInfo.h"

#define SERVER_IP "127.0.0.1"
#define FILE_OPEN_ERR -1
#define FILE_LENGTH_RECV_ERR -2
#define FILE_RECEIVE_ERR -3

class ServerDTPModule
{
private:
    std::string original_path;
    std::vector<std::string> check_sums;
    std::string server_home_path;
    const std::string SEND_FILE_COMMAND = "send ";
    std::vector<std::thread> threads;
public:
    std::string ManageCommand(std::string command, ClientInfo* info);
    std::string getHomePath() { return server_home_path; }
    void SetPath(std::string path);
    static inline std::string SHA256(const char* const filepath);
private:
    void traverseAllFilesAndCalculateCheckSums();
    std::string CommandLs(ClientInfo* info);
    std::string CommandCd(std::string folder, ClientInfo* info);    
    std::string CommandLogin(std::string username, ClientInfo* info);
    std::string CommandLogMe(std::string sentInfo, ClientInfo* info);
    std::string CommandLogout(ClientInfo* info);
    std::string CommandSignin();
    std::string CommandCreateAccount(std::string account, ClientInfo* info);
    std::string CommandSend(std::string fileName, ClientInfo* clientInfo);
    std::string CheckIfFileExists(const std::string client_sha_code);
    void handleReceive(int sock_fd);
    int receiveBuffer(int sock_fd, char* buffer, int bufferSize, int chunkSize = 4 * 1024);
    long receiveFile(int sock_fd, const std::string& fileName, int chunkSize = 64 * 1024);
    bool isDirectory(std::string path);
    bool isFile(std::string path);
    bool isFileAlreadyOnServer(std::string path);
};
