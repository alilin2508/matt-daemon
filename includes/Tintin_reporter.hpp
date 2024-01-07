#ifndef TINTIN_REPORTER_HPP
# define TINTIN_REPORTER_HPP

#include <iostream>
#include <fstream>
#include <ctime>

class Tintin_reporter
{
public:
	Tintin_reporter();
	void logs(const std::string& message);
	~Tintin_reporter();
private:
	bool fileExists(const char* filename);

};

#endif
