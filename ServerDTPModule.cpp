#include "ServerDTPModule.h"
#include <fstream>
#include "FTP.h"

namespace fs = std::filesystem;

using std::string;
using std::thread;
using std::min;
using std::ofstream;
using std::cout;
using std::endl;

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

    if(command.find("login ") == 0)
    {
        return CommandLogin(command.substr(6, command.length()), info);
    }

    if(command.find("log me ") == 0)
    {
        return CommandLogMe(command.substr(7, command.length()), info);
    }

    if(command == "logout")
    {
        return CommandLogout(info);
    }

    if(command == "sign")
    {
        return CommandSignin();
    }

    if(command.find("new account ") == 0)
    {
        return CommandCreateAccount(command.substr(12, command.length()), info);
    }

    if(command.find(SEND_FILE_COMMAND) == 0) {
        return CommandSend(command.substr(SEND_FILE_COMMAND.length(), command.length()), info);
    }

    if(command.length() == FTP::SHA256_LENGTH){
        return CheckIfFileExists(command);
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
    std::cout << "siema loginek leci" << std::endl;
    //std::cout << "otrzymany username:" + username + "kuniec" << std::endl;
    std::ifstream clientsData;
    clientsData.open(server_home_path + "ClientsData.txt", std::fstream::in);
    std::cout << server_home_path << std::endl;
    if(!clientsData)
        std::cout << "plik elegancko nie otwarty" << std::endl;
    std::string login;
    while(clientsData >> login)
    {
        std::cout << login << std::endl;
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
        //std::cout << tmp << "yy" << std::endl;
        if(tmp == username)
            return "error An user with this username already exists!";
        else
            clientsData >> tmp;
    }
    clientsData.close();
    clientsData.open(server_home_path + "RegistrationRequests.txt", std::fstream::app);
    clientsData << username << " " << password << "\n";
    clientsData.close();
    return "show Your request was sent to admin. You have to wait until request is accepted.";
}


string ServerDTPModule::CommandSend(string fileName, ClientInfo* clientInfo) {
    int sock_fd;
    struct sockaddr_in serv_addr;
    int port = 0;
    const int ONE_CONNECTION = 1;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("DTP module failed to create a socket");
        return nullptr;
    };

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("DTP module failed to bind address to socket");
        return nullptr;
    }

    struct sockaddr_in add;
    size_t add_len = sizeof(add);
    if(getsockname(sock_fd, (struct sockaddr *)&add,  (socklen_t*) &add_len) != 0)
    {
        perror("DTP modeule failed binding socket");
        return nullptr;
    }

    listen(sock_fd, ONE_CONNECTION);

    threads.push_back(thread(&ServerDTPModule::handleReceive, this, sock_fd));
    std::cout << sock_fd;
    return "ConnectTo " + std::to_string(add.sin_port);
}

string ServerDTPModule::CheckIfFileExists(string client_sha_code){
    for( auto iter = check_sums.begin(); iter != check_sums.end(); ++iter){
        if(*iter == client_sha_code)
            return "file on server";
    }

    return "file not on server";
}

void ServerDTPModule::handleReceive(int sock_fd) {
        std::cout << sock_fd;
        printf("Waiting for connection...\n");

        struct sockaddr_in client_addr;
        size_t client_len = sizeof(client_addr);
        std::cout << "nie yoo\n\n";
        int client_socket;
        if ((client_socket = accept(sock_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_len)) < 0) {
            perror("DTP failed while executing accept()");
        };

        long rc = receiveFile(client_socket, "send.txt");
        if (rc < 0) {
            cout << "Failed to receive file: " << rc << endl;
        } else {
            cout << "File receive success";
        }


        std::cout << "yoooo\n\n";
        shutdown(client_socket, SHUT_RDWR);
        shutdown(sock_fd, SHUT_RDWR);
        close(client_socket);
        close(sock_fd);
}

int ServerDTPModule::receiveBuffer(int sock_fd, char* buffer, int bufferSize, int chunkSize) {
    int i = 0;
    while (i < bufferSize) {
        const int l = recv(sock_fd, &buffer[i], min(chunkSize, bufferSize - i), 0);
        if (l < 0) {
            return -1;
        }
        i += l;
    }

    return 1;
}

long ServerDTPModule::receiveFile(int sock_fd, const string& fileName, int chunkSize) {
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

    cout << "jest" << endl;

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

bool ServerDTPModule::isDirectory(std::string path)
{
    return fs::is_directory(fs::status(path));
}

bool ServerDTPModule::isFile(std::string path)
{
    return fs::is_regular_file(fs::status(path));
}
