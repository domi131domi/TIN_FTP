#include "ClientPIModule.h"

namespace fs = std::filesystem;

using std::string;

ClientPIModule::ClientPIModule(int port, std::string ip_str)
{
    struct sockaddr_in serv_addr;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(ip_str == "0" || ip_str == "localhost")
        ip_str = LOCALHOST;
    if(inet_pton(AF_INET, ip_str.c_str(), &serv_addr.sin_addr)<=0)
    {
        perror("Invalid adress");
        return;
    }

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return;
    }

    isRunning = true;
}

bool ClientPIModule::SendCommand(std::string msg)
{
    if(!isRunning)
        return false;

    if( msg.find(SEND_FILE_COMMAND) == 0 ) {
        if (isFileAlreadyOnServer(msg) )
            return true;
        
        file_to_send = msg.substr(SEND_FILE_COMMAND.length(), msg.length());
    }

    if ( msg.find(GET_FILE_COMMAND) == 0 ) {
        file_to_receive = msg.substr(GET_FILE_COMMAND.length(), msg.length());
    }

    if( !FTP::Send(client_socket, msg) )
        return false;

    std::string reply = FTP::Read(client_socket);
    if( reply.length() == 0 )
        return false;

    ManageReply(reply);
    return true;
}

bool ClientPIModule::isFileAlreadyOnServer(std::string msg){
    bool ERROR_OCCURED = false;
    std::string file_name = msg.erase(0, SEND_FILE_COMMAND.length() );
    file_name.erase(0, file_name.find_first_not_of(' '));

    if( !fs::is_regular_file(file_name) ){
        std::cout << "File doesn't exists!\n";
        return true;
    }

    std::string file_sha256;
    try{
        file_sha256 = FTP::SHA256( file_name.c_str() );
    } catch (const std::exception& e) {
        std::cerr <<  e.what() << "\n";
    }

    if( !FTP::Send(client_socket, file_sha256.c_str()) )
       throw ERROR_OCCURED;

    std::string reply = FTP::Read(client_socket);
    if( reply == "" )
        throw ERROR_OCCURED;

    if( reply == "file on server" ){
        std::cout << "The same file is already on server\n";
        return true;
    }

    return false;
}

void ClientPIModule::Close()
{
    close(client_socket);
}

void ClientPIModule::ManageReply(std::string reply)
{
    if(reply.find("setPath ") == 0)
    {
        current_path = "~/" + reply.substr(8, reply.length());
    }
    else if(reply.find("error ") == 0)
    {
        std::cout << reply.substr(6, reply.length()) << std::endl;
    }
    else if(reply.find("show ") == 0)
    {
        std::cout << reply.substr(5, reply.length()) << std::endl;
    }
    else if(reply.find("send password ") == 0)
    {
        //std::cout << "oooj bedzie logowane " << std::endl;
        if(isLogged)
            std::cout << "You are already logged in." << std::endl;
        else
        {
            std::string username = reply.substr(14, reply.length());
            std::cout << "password: ";
            std::string typedPassword;
            getline(std::cin, typedPassword);
            SendCommand("log me " + username + "\n" + typedPassword);
        }
    }
    else if(reply.find("login result ") == 0)
    {
        std::string answer = reply.substr(13, reply.length());
        if(answer == "You have been logged in successfully!")
            isLogged = true;
        std::cout << answer << std::endl;
    }
    else if(reply.find("logout result ") == 0)
    {
        isLogged = false;
        std::cout << reply.substr(14, reply.length()) << std::endl;
    }
    else if(reply == "signin")
    {
        std::string username;
        std::string password;
        std::cout << "username: ";
        getline(std::cin, username);
        std::cout << "password: ";
        getline(std::cin, password);
        SendCommand("new account " + username + "\n" + password);
    }

    else if(reply.find("ConnectTo ") == 0) {
        clientDTPModule->proceedSend(std::stoi(reply.substr(10,reply.length())), file_to_send);
    }

    else if(reply.find(GET_ACCEPT_RESPONSE)) {
        clientDTPModule->handleReceive(file_to_receive);
    }
}

std::string ClientPIModule::GetPath()
{
    return current_path;
}

void ClientPIModule::connectToDTP(ClientDTPModule* address) {
    clientDTPModule = address;
}
