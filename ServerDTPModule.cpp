#include "ServerDTPModule.h"
#include <fstream>
#include "FTP.h"

namespace fs = std::filesystem;

using std::string;
using std::thread;
using std::min;
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;

std::string ServerDTPModule::ManageCommand(std::string command, ClientInfo* info)
{
    std::cout << "Managing command " << command << std::endl;
    if(command == LS)
    {
        return CommandLs(info);
    }

    if(command.find(CD) == 0)
    {
        return CommandCd(command.substr(CD.length(), command.length()), info);
    }

    if(command.find(LOGIN) == 0)
    {
        return CommandLogin(command.substr(LOGIN.length(), command.length()), info);
    }

    if(command.find(LOGME) == 0)
    {
        return CommandLogMe(command.substr(LOGME.length(), command.length()), info);
    }

    if(command == LOGOUT)
    {
        return CommandLogout(info);
    }

    if(command == SIGN)
    {
        return CommandSignin();
    }

    if(command.find(NEW_ACC) == 0)
    {
        return CommandCreateAccount(command.substr(NEW_ACC.length(), command.length()), info);
    }

    if(command.find(SEND_FILE_COMMAND) == 0) {
        return CommandSend(command.substr(SEND_FILE_COMMAND.length(), command.length()), info);
    }

    if(command.length() == FTP::SHA256_LENGTH){
        return CheckIfFileExists(command);
    }

    if(command.find(MAKEDIR) == 0)
    {
        return CommandMkdir(command.substr(MAKEDIR.length(), command.length()), info);
    }

    if(command.find(RM) == 0)
    {
        return CommandRm(command.substr(RM.length(), command.length()), info);
    }

    if (command.find(GET_FILE_COMMAND) == 0) {
        return CommandGet(command.substr(GET_FILE_COMMAND.length(), command.length()), info);
    }
    return "error command unknown";
}

void ServerDTPModule::SetPath(std::string path)
{
    std::cout << "Setting path: " << path << std::endl;
    original_path = path;
    std::string tmp = original_path.substr(0, original_path.length()-1);
    std::size_t found = tmp.find_last_of("/");
    server_home_path = original_path.substr(0, found+1);
    /*for (const auto& entry : fs::directory_iterator(path))
        files.push_back(entry.path().string().substr(original_path.length(), entry.path().string().length()));*/
    traverseAllFilesAndCalculateCheckSums();
}

void ServerDTPModule::traverseAllFilesAndCalculateCheckSums(){
    for (auto file : fs::recursive_directory_iterator(original_path)) {
        try {
             check_sums.push_back( FTP::SHA256(file.path().c_str()) );
        } catch(const std::exception& e) {
            std::cerr <<  e.what() << "\n";
        }
    }
}

std::string ServerDTPModule::CommandLs(ClientInfo* info)
{
    if(!info->isLogged)
        return "error You have to log in first see the files!";
    std::string ret = "show ";
    std::string path = original_path + info->currentRelativePath;
    for (const auto& entry : fs::directory_iterator(path))
        ret += entry.path().string().substr(path.length(), entry.path().string().length()) + " ";
    return ret;
}

std::string ServerDTPModule::CommandCd(std::string path, ClientInfo* info)
{
    if(!info->isLogged)
        return "error You have to log in first to surf the files!";
    std::cout << "command cd argument: " << original_path + info->currentRelativePath + path << std::endl;
    //std::cout << original_path << std::endl << "ee" << info->currentRelativePath << "ee" << std::endl << "ppp" << path  << "ppp" <<std::endl;
    if(!isDirectory(original_path + info->currentRelativePath + path))
        return "error Folder doesnt exist!";

    if (path == ".." && info->currentRelativePath != ""){
        std::string tmp = info->currentRelativePath;
        tmp = tmp.substr(0, tmp.size() - 1);    // removes last "/"
        std::cout << "checking .." << tmp << std::endl;
        std::size_t found = tmp.find_last_of("/");
        tmp = tmp.substr(0, found + 1);
        std::cout << "checking2 .." << tmp << std::endl;
        info->currentRelativePath = tmp;
    }
    else if(!(path == ".." && info->currentRelativePath == ""))
        info->currentRelativePath = info->currentRelativePath + path + "/";
    return "setPath " + info->currentRelativePath;
}

std::string ServerDTPModule::CommandLogin(std::string username, ClientInfo* info)
{
    //std::cout << "siema loginek leci" << std::endl;
    //std::cout << "otrzymany username:" + username + "kuniec" << std::endl;
    std::ifstream clientsData;
    clientsData.open(server_home_path + "ClientsData.txt", std::fstream::in);
    std::cout << server_home_path << std::endl;
    if(!clientsData)
        std::cout << "plik elegancko nie otwarty" << std::endl;
    std::string login;
    while(clientsData >> login)
    {
        if(login == username)
        {
            clientsData.close();
            return "send password " + username;
        }
        else
        {
            clientsData >> login;   // skip this password
            continue;
        }
    }
    clientsData.close();
    clientsData.open(server_home_path + "RegistrationRequests.txt", std::fstream::in);
        while(clientsData >> login)
    {
        if(login == username)
        {
            clientsData.close();
            return "show Your request to create account has not been accepted yet.";
        }
        else
        {
            clientsData >> login;   // skip this password
            continue;
        }
    }
    clientsData.close();
    return "error Invalid username. Did you mean to sign in? Use \"sign\" command instead.";
}

std::string ServerDTPModule::CommandLogMe(std::string sentInfo, ClientInfo* info)
{
    std::size_t endline = sentInfo.find("\n");
    std::string username = sentInfo.substr(0, endline);
    std::string typedPassword = sentInfo.substr(endline+1, sentInfo.length());
    std::ifstream clientsData;
    clientsData.open(server_home_path + "ClientsData.txt", std::fstream::in);
    std::string login;
    std::string expectedPassword;
    while(clientsData >> login)
    {
        if(login == username)
        {
            clientsData >> expectedPassword;
            if(typedPassword == expectedPassword)
            {
                info->isLogged = true;
                clientsData.close();
                return "login result You have been logged in successfully!";
            }
            else
            {
                clientsData.close();
                return "login result Invalid password.";
            }
        }
        else
            clientsData >> login;
    }
    clientsData.close();
    return "error Error in loging in.";
}

std::string ServerDTPModule::CommandLogout(ClientInfo* info)
{
    info->isLogged = false;
    return "logout result You have been logged out successfully.";
}

std::string ServerDTPModule::CommandSignin()
{
    return "signin";
}

std::string ServerDTPModule::CommandCreateAccount(std::string account, ClientInfo* info)
{
    std::size_t endline = account.find("\n");
    std::string username = account.substr(0, endline);
    std::string password = account.substr(endline+1, account.length());
    std::fstream clientsData;
    clientsData.open(server_home_path + "ClientsData.txt", std::fstream::in);
    std::string tmp;
    while(clientsData >> tmp)
    {
        if(tmp == username){
            clientsData.close();
            return "error An user with this username already exists!";
        }
        else
            clientsData >> tmp;
    }
    clientsData.close();
    clientsData.open(server_home_path + "RegistrationRequests.txt", std::fstream::in);
    while(clientsData >> tmp)
    {
        if(tmp == username){
            return "error An user with this username already exists!";
        }
        else
            clientsData >> tmp;
    }
    clientsData.close();
    clientsData.open(server_home_path + "RegistrationRequests.txt", std::fstream::app);
    clientsData << username << " " << password << "\n";
    clientsData.close();
    return "show Your request was sent to admin. You have to wait until request is accepted.";
}


string ServerDTPModule::CommandSend(const string& fileName, ClientInfo* clientInfo) {
    int sock_fd = createServerSocket();
    struct sockaddr_in add;
    size_t add_len = sizeof(add);
    if(getsockname(sock_fd, (struct sockaddr *)&add,  (socklen_t*) &add_len) != 0)
    {
        perror("DTP modeule failed binding socket");
        return nullptr;
    }

    const string fullFilePath = original_path + clientInfo->currentRelativePath + fileName;
    if( isDirectory(fullFilePath) ){
        return "error Sending directories is forbidden";
    }

    threads.push_back(thread(&ServerDTPModule::handleReceive, this, sock_fd, fullFilePath));

    return "ConnectTo " + std::to_string(add.sin_port);
}

std::string ServerDTPModule::CommandMkdir(std::string folderName, ClientInfo* info) {
    if(isDirectory(original_path + info->currentRelativePath + folderName + '/'))
        return "error Such forler already exists in this directory.";

    if(isFile(original_path + info->currentRelativePath + folderName))
        return "error You cannot create folder named same as file in this context.";

    else if(fs::create_directory(original_path + info->currentRelativePath + folderName + '/'))
            return "show Folder created.";

    return "error Error in creating new directory.";
}

std::string ServerDTPModule::CommandRm(std::string removed, ClientInfo* info) {
    const string fullFilePath = original_path + info->currentRelativePath + removed;
    if( isFile(fullFilePath) ){
        std::string sha256 = FTP::SHA256(fullFilePath.c_str());
        auto position = std::find(check_sums.begin(), check_sums.end(), sha256);
        if( position != check_sums.end() ){
            check_sums.erase( position );
        }
        fs::remove( fullFilePath );
        return "show File was successfuly removed";
    }

    else if ( isDirectory(fullFilePath) ){
        try {
            for (auto file : fs::recursive_directory_iterator(fullFilePath)) {
                std::string sha256 = FTP::SHA256(file.path().c_str());
                auto position = std::find(check_sums.begin(), check_sums.end(), sha256);
                if( position != check_sums.end() ){
                    check_sums.erase( position );
                }
            }
        } catch(const std::exception& e) {
            std::cerr <<  e.what() << "\n";
            return "error There was a problem during managing command";
        }

        std::uintmax_t n = fs::remove_all(fullFilePath);
        return "show Removed " + std::to_string(n) + " files or directories.";
    }

    else{
        return "error There is no such file in directory";
    }


}

string ServerDTPModule::CheckIfFileExists(string client_sha_code){
    for( auto iter = check_sums.begin(); iter != check_sums.end(); ++iter){
        if(*iter == client_sha_code)
            return "file on server";
    }

    return "file not on server";
}

void ServerDTPModule::handleReceive(int sock_fd, const string fileName) {
        printf("Waiting for connection...\n");

        struct sockaddr_in client_addr;
        size_t client_len = sizeof(client_addr);

        int client_socket;
        if ((client_socket = accept(sock_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_len)) < 0) {
            perror("DTP failed while executing accept()");
        };

        long rc = FTP::receiveFile(client_socket, fileName);
        if (rc < 0) {
            cout << "Failed to receive file: " << rc << endl;
        } else {
            cout << "File receive success";
            check_sums.push_back( FTP::SHA256(fileName.c_str()) );
        }

        close(client_socket);
        close(sock_fd);
}

string ServerDTPModule::CommandGet(const string& fileName, ClientInfo* clientInfo) {
    int sock_fd = createServerSocket();
    if (sock_fd < 0) {
        return "error Could not create server socket";
    }

    const string fullFilePath = original_path + clientInfo->currentRelativePath + fileName;
    if( isDirectory(fullFilePath) ){
        return "error Getting directories is forbidden";
    }

    struct sockaddr_in add;
    size_t add_len = sizeof(add);
    if(getsockname(sock_fd, (struct sockaddr *)&add,  (socklen_t*) &add_len) != 0)
    {
        perror("DTP modeule failed binding socket");
        return nullptr;
    }


    threads.push_back(thread(&ServerDTPModule::proceedSend, this, sock_fd, fullFilePath));

    return GET_ACCEPT_RESPONSE + std::to_string(add.sin_port);
}

void ServerDTPModule::proceedSend(int sock_fd, string fileName) {
        printf("Waiting for connection...\n");

        struct sockaddr_in client_addr;
        size_t client_len = sizeof(client_addr);

        int client_socket;
        if ((client_socket = accept(sock_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_len)) < 0) {
            perror("DTP failed while executing accept()");
        };

        long rc = FTP::sendFile(client_socket, fileName);
        if (rc < 0) {
            cout << "Failed to send file: " << rc << endl;
        }

        cout << "Sending file done" << endl;

        close(client_socket);
        close(sock_fd);

        cout << "Sending file done2" << endl;
}

bool ServerDTPModule::isDirectory(std::string path)
{
    return fs::is_directory(fs::status(path));
}

bool ServerDTPModule::isFile(std::string path)
{
    return fs::is_regular_file(fs::status(path));
}

int ServerDTPModule::createServerSocket() {
    int sock_fd;
    struct sockaddr_in serv_addr;
    int port = 0;
    const int ONE_CONNECTION = 1;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("DTP module failed to create a socket");
        return -1;
    };


    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setting options error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("DTP module failed to bind address to socket");
        return -1;
    }

    listen(sock_fd, ONE_CONNECTION);

    return sock_fd;
}
