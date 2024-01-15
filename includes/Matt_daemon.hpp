#ifndef MATT_DAEMON_HPP
# define MATT_DAEMON_HPP

# include "Tintin_reporter.hpp"
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/epoll.h>
# include <stdio.h>
# include <stdlib.h>
# include <cstring>
# include <sys/stat.h>
# include <signal.h>
# include <vector>
# include <algorithm>
# include <iostream>
# include <sysexits.h>

# define MAX_EVENTS 300
# define PORT 4242
# define MAXCONN 3

using namespace std;

class Matt_daemon
{
public:
	Matt_daemon();
	~Matt_daemon();
  	static Matt_daemon*& instance() {
        static Matt_daemon* inst = nullptr;
        return inst;
  	}
  	void start(); 
	void stop();
	int 	set_nonblock(int fd);
	void	close_server();
	static void signalHandler(int sig);
	void	unlockDaemon();
private:
	int					MasterSocket;
	std::vector<int>	ClientSocket;
	int					EPoll;
	struct epoll_event	Events[MAX_EVENTS];
	struct epoll_event	Event;
	Tintin_reporter 	myReporter;
};

#endif
