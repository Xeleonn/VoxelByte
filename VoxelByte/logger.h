#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>

class Logger
{
public:
	Logger();

	std::string GetCurrentTime();

	void Print(std::string message);
	void PrintErr(std::string message);
};

#endif