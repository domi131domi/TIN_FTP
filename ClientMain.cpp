#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "ClientPIModule.h"
#define PORT 8080

int main(int argc, char const *argv[])
{
    if(argc < 2){
        std::cout << "Program needs a server port as an argument" << std::endl;
        return 0;
    }

    int port = atoi(argv[1]);
    ClientPIModule client(port, "127.0.0.1");
    if( !client.isRunning )
        return 0;

    ClientDTPModule dtp(port);
    client.connectToDTP(&dtp);
    std::string text = "";
    std::cout <<
            " __          __  _                            _          ______ _______ _____     _____                          \n" 
            " \\ \\        / / | |                          | |        |  ____|__   __|  __ \\   / ____|                         \n" 
            "  \\ \\  /\\  / /__| | ___ ___  _ __ ___   ___  | |_ ___   | |__     | |  | |__) | | (___   ___ _ ____   _____ _ __ \n" 
            "   \\ \\/  \\/ / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  |  __|    | |  |  ___/   \\___ \\ / _ \\ '__\\ \\ / / _ \\ '__|\n" 
            "    \\  /\\  /  __/ | (_| (_) | | | | | |  __/ | || (_) | | |       | |  | |       ____) |  __/ |   \\ V /  __/ |   \n" 
            "     \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/  |_|       |_|  |_|      |_____/ \\___|_|    \\_/ \\___|_|   \n" 
            "                                                                                                                 \n" 
            "                                                                                                                 \n";
    while(true)
    {
        std::cout << client.GetPath() + "$ ";
        getline(std::cin, text);
        if(text == "quit")
        {
            client.Close();
            break;
        }
        if(!client.SendCommand(text))
            return 0;
    }
    return 0;
}
