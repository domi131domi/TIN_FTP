#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

class Command
{
public:
	std::string commandName;
	std::vector<std::string> arguments;

	Command(std::string text);
	Command();
};
