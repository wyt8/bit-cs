#pragma once
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>

class Logger
{
public:
	static void createLogFile(std::string name);

	static void closeLogFile();
		 
	static void write(std::string line);
	
private:
	static bool isCreated;
	static std::ofstream logFile;
};


