#ifndef TINTIN_REPORTER_HPP
# define TINTIN_REPORTER_HPP

#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

class Tintin_reporter
{
public:
	Tintin_reporter();
	void logs(const std::string& message, const char* mode);
	~Tintin_reporter();
private:
	Tintin_reporter(Tintin_reporter const &other);
	Tintin_reporter &operator=(Tintin_reporter const &other);
	void openOrCreate(const std::string& filename);
	void closeStream();
	void printFile(const std::string& message, const char* formatTime, const char* mode);
	std::ofstream streamLogFile;

friend class Matt_daemon;
};

#endif