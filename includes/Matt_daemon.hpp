#ifndef MATT_DAEMON_HPP
# define MATT_DAEMON_HPP

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

	int 	set_nonblock(int fd);
	void	close_server();
private:
	int					MasterSocket;
	std::vector<int>	ClientSocket;
	int					EPoll;
	struct epoll_event	Events[MAX_EVENTS];
	struct epoll_event	Event;
};


#endif