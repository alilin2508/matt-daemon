#include "Matt_daemon.hpp"


int main()
{
	//check getuid()
	//check matt_daemon.lock doeas not exist
	// Create a daemon
	Matt_daemon daemon;

	// Start the daemon
	daemon.start();

	// Wait for the daemon to stop
	daemon.wait();

	return 0;
}
