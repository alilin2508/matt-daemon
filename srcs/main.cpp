#include "Matt_daemon.hpp"
#include <sys/stat.h>
#include <sys/file.h>

void generateKillScript(const std::string& filename) {
    std::ofstream scriptFile(filename, std::ofstream::trunc);

    if (scriptFile.is_open()) {
        scriptFile << "#!/bin/bash\n"
                   << "PIDS=$(pgrep -x \"Matt_daemon\")\n"
                   << "if [ $(echo \"$PIDS\" | wc -w) -gt 1 ]\n"
                   << "then\n"
                   << "    killall -15 Matt_daemon\n"
				   << "	   echo \"Some idiot deleted the lockfile - killing all Matt_daemon process now\"\n"
				   << "	   echo \"You have to restart it\"\n"
                   << "    sleep 1\n"
                   << "    for PID in $PIDS\n"
                   << "    do\n"
                   << "        if ps -p $PID > /dev/null\n"
                   << "        then\n"
                   << "            echo \"Failed to terminate Matt_daemon (PID: $PID)\"\n"
                   << "        else\n"
                   << "            echo \"Termination signal sent to Matt_daemon (PID: $PID)\"\n"
                   << "        fi\n"
                   << "    done\n"
                   << "else\n"
                   << "    echo \"Can start new Matt_daemon process - no one exist\"\n"
                   << "fi\n";

        scriptFile.close();
		chmod(filename.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    } else {
        std::cerr << "Error creating script file: " << filename << std::endl;
    }
}

bool	daemon_running(const std::string& filename, int *lockFileFdPtr) {
	struct stat buffer;

	generateKillScript("/kill_daemon.sh");
	//If file doesn't exist kill process if it exist
	if (stat(filename.c_str(), &buffer) != 0) {
		//std::cerr << "File lock don't exist : " << errno << std::endl;
		//Warning if delete lock file by hand all process is killed here in the script
		std::system("/kill_daemon.sh");
		//We create the file only so we close after
		*lockFileFdPtr = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
		if (*lockFileFdPtr == -1) {
			perror("Error opening or creating lock file");
			return false;
		}
		close(*lockFileFdPtr);
	}

	//If file exist Get the fd to lock
	//std::cerr << "File lock exist : " << errno << std::endl;
	*lockFileFdPtr = open(filename.c_str(), O_RDWR | O_NONBLOCK);
	if (flock(*lockFileFdPtr, LOCK_EX | LOCK_NB) == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			std::ofstream streamLogFile;
			streamLogFile.open("/var/log/matt_daemon", std::ios::out | std::ios::app);
			std::time_t now = std::time(0);
			std::tm* localTime = std::localtime(&now);
			char buffer[80];
			strftime(buffer, sizeof(buffer), "[%d/%m/%Y-%H:%M:%S]", localTime);
			streamLogFile << buffer << " [ ERROR ] - Matt_daemon : File is already locked " << std::endl;
			streamLogFile.close();
			std::cerr << "File is already locked: " << errno << std::endl;
		} else {
			perror("Error locking file");
		}
		close(*lockFileFdPtr);
		return true;
	}
	std::cout << "Obtained lock." << std::endl;
	return false;
}

int main()
{
	int lockFileFd;
	if (getuid() != 0)
	{
		std::cout << "User launching the daemon server is not root" << std::endl;
		return (1);
	}
	if (daemon_running("/var/lock/matt_daemon.lock", &lockFileFd)) {
		std::cout << "Matt_daemon is already running" << std::endl;
		std::cout << "use : ps aux | grep Matt_daemon to figure out the PID process" << std::endl;
		return 1;
	}
	Matt_daemon daemon(lockFileFd);
	daemon.start();
	daemon.stop();
	return 0;
}
