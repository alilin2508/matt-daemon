#ifndef TINTIN_REPORTER_HPP
# define TINTIN_REPORTER_HPP

#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>
#include "Matt_daemon.hpp"

class Tintin_reporter
{
public:
	Tintin_reporter();
	void logs(const std::string& message);
	~Tintin_reporter();
private:
	void printFile();
	void openOrCreate(std::ofstream filestream, const std::string& filename, const char* mode)
	void printFile(std::ofstream filestream, const std::string& filename, const std::string& message, const char* formatTime, const char* mode) {

friend class Matt_daemon
};

#endif
