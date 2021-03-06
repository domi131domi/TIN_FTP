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

class ServerDTPModule
{
private:
    std::string original_path;
    std::vector<std::string> check_sums;
    std::string server_home_path;
    const std::string SEND_FILE_COMMAND = "send ";
    std::vector<std::thread> threads;
    const std::string GET_FILE_COMMAND = "get ";
    const std::string GET_ACCEPT_RESPONSE = "get_accept ";
    const std::string LS = "ls";
    const std::string CD = "cd ";
    const std::string LOGIN = "login ";
    const std::string LOGME = "log me ";
    const std::string LOGOUT = "logout";
    const std::string SIGN = "sign";
    const std::string NEW_ACC = "new account ";
    const std::string MAKEDIR = "mkdir ";
    const std::string RM = "rm ";
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
    std::string CommandMkdir(std::string directory, ClientInfo* clientInfo);
    std::string CommandRm(std::string removed, ClientInfo* info);
    std::string CheckIfFileExists(const std::string client_sha_code);
    std::string CommandSend(const std::string& fileName, ClientInfo* clientInfo);
    std::string CommandGet(const std::string& fileName, ClientInfo* clientInfo);
    void handleReceive(int sock_fd, const std::string fileName);
    void proceedSend(int sock_fd, const std::string fileName);
    bool isDirectory(std::string path);
    bool isFile(std::string path);
    bool isFileAlreadyOnServer(std::string path);
    int createServerSocket();
};
