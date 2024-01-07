#ifndef MATT_DAEMON_HPP
# define MATT_DAEMON_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include "Tintin_reporter.hpp"

# define MAX_EVENTS 3
# define PORT 4242

class Matt_daemon
{
public:
	Matt_daemon();
	~Matt_daemon();

	int set_nonblock(int fd);
private:
	Tintin_reporter myReporter
	std::ofstream streamLogFile;
};

#endif
