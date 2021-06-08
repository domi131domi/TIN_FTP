#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "ClientPIModule.h"
#define PORT 8080

int main(int argc, char const *argv[])
{
    ClientPIModule client(8080, "127.0.0.1");
    std::string text = "";
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
