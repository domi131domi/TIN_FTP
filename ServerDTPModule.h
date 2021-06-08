#pragma once
#include <string>
#include <experimental/filesystem>
#include <iostream>
#include "ClientInfo.h"

class ServerDTPModule
{
private:
    std::string original_path;
public:
    std::string ManageCommand(std::string command, ClientInfo* info);
    void SetPath(std::string path);
private:
    std::string CommandLs(ClientInfo* info);
    std::string CommandCd(std::string folder, ClientInfo* info);
    bool isDirectory(std::string path);
    bool isFile(std::string path);
};
