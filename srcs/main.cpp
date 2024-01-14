//#include "Tintin_reporter.hpp"
#include "Matt_daemon.hpp"
#include <sys/stat.h>

bool	daemon_running(const std::string& filename) {
	struct stat buffer;
	if (stat(filename.c_str(), &buffer) == 0){
		return true;
	} else {
		std::ofstream streamLock (filename);
		streamLock << "deamon running" << std::endl;
		streamLock.close();
	}
	return false;
}

int main()
{
	// if (getuid() != 0)
	// {
	// 	std::cout << "User launching the daemon server is not root" << std::endl;
	// 	return (1);
	// }
	//check matt_daemon.lock doeas not exist
	// Create a daemon
	if (daemon_running("/var/lock/matt_daemon.lock")) {
		std::cout << "Matt_daemon is already running" << std::endl;
		std::cout << "use : ps aux | grep Matt_daemon to figure out the PID process" << std::endl;
		return 1;
	}
	Matt_daemon daemon;
	daemon.start();
	daemon.stop();
	return 0;
}
