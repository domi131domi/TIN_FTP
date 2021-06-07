#include "Command.h"

Command::Command(std::string text)
{
	std::istringstream iss(text);
    std::string word;
    iss >> commandName;
    while(iss >> word)
    {
        arguments.push_back(word);
    }
}

Command::Command()
{

}
