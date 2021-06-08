#include "ServerDTPModule.h"
namespace fs = std::experimental::filesystem;

std::string ServerDTPModule::ManageCommand(std::string command, ClientInfo* info)
{
    std::cout << "Managing command " << command << std::endl;
    if(command == "ls")
    {
        return CommandLs(info);
    }

    if(command.find("cd ") == 0)
    {
        return CommandCd(command.substr(3, command.length()), info);
    }
    return "error command unknown";
}

void ServerDTPModule::SetPath(std::string path)
{
    std::cout << "Setting path: " << path << std::endl;
    original_path = path;
    /*for (const auto& entry : fs::directory_iterator(path))
        files.push_back(entry.path().string().substr(original_path.length(), entry.path().string().length()));*/
}

std::string ServerDTPModule::CommandLs(ClientInfo* info)
{
    std::string ret = "show ";
    std::string path = original_path + info->currentRelativePath;
    for (const auto& entry : fs::directory_iterator(path))
        ret += entry.path().string().substr(path.length(), entry.path().string().length()) + " ";
    return ret;
}

std::string ServerDTPModule::CommandCd(std::string path, ClientInfo* info)
{
    std::cout << "command cd argument: " << original_path + info->currentRelativePath + path << std::endl;
    if(!isDirectory(original_path + info->currentRelativePath + path))
        return "error Folder doesnt exist!";
    info->currentRelativePath = info->currentRelativePath + path + "/";
    return "setPath " + info->currentRelativePath;
}

bool ServerDTPModule::isDirectory(std::string path)
{
    return fs::is_directory(fs::status(path));
}

bool ServerDTPModule::isFile(std::string path)
{
    return fs::is_regular_file(fs::status(path));
}
