#include "Matt_daemon.hpp"
#include "Tintin_reporter.hpp"
#include <sys/file.h>

int Matt_daemon::set_nonblock(int fd)
{
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Matt_daemon::close_server()
{
    for (int clientSocket : ClientSocket)
    {
        epoll_ctl(EPoll, EPOLL_CTL_DEL, clientSocket, NULL);
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }
    myReporter.logs("Exiting Daemon mode.", "INFO");
	myReporter.closeStream();
	unlockDaemon();
    close(MasterSocket);
    close(EPoll);
    exit(EXIT_SUCCESS);
}

void Matt_daemon::unlockDaemon() {
    if (flock(fdLock, LOCK_UN) == -1) {
        perror("Error unlocking file");
    } else {
        std::cout << "File unlocked successfully." << std::endl;
        std::remove("/var/lock/matt_daemon.lock");
    }
    close(fdLock);
}

void Matt_daemon::signalHandler(int sig)
{	
    Matt_daemon* myDaemon = instance();

    myDaemon->myReporter.logs("Signal received: " + std::to_string(sig), "INFO");
    myDaemon->close_server();
    myDaemon->unlockDaemon();
	exit(EXIT_SUCCESS);
}

Matt_daemon::Matt_daemon(int lockFileFd) : fdLock(lockFileFd) {
    std::cout << "Constructor Matt_daemon called, fdLock assigned to " << fdLock << std::endl;
}

Matt_daemon::Matt_daemon() {
    std::cout << "Constructor Matt Daemon called" << std::endl;
}


void Matt_daemon::stop() {
    instance() = nullptr;
}

void Matt_daemon::start()
{
	instance() = this;
	signal(SIGCHLD, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGKILL, signalHandler);

    pid_t pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) {
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        exit(EXIT_SUCCESS);
    }

	pid_t daemonPid = getpid();
    char daemonPidStr[30];
    snprintf(daemonPidStr, sizeof(daemonPidStr), "%s%d", "started. PID: ", daemonPid);

    umask(0);
		
	myReporter.openOrCreate("/var/log/matt_daemon");
	myReporter.logs("Entering Daemon mode.", "INFO");
	myReporter.logs(daemonPidStr, "INFO");

    MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(PORT);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));

    set_nonblock(MasterSocket);

    listen(MasterSocket, MAXCONN);

    EPoll = epoll_create1(0);

    Event.data.fd = MasterSocket;
    Event.events = EPOLLIN;
    epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);

    while (1) {
        int N = epoll_wait(EPoll, Events, MAX_EVENTS, -1);

        for (int i = 0; i < N; ++i) {
            if (Events[i].data.fd == MasterSocket) {
                if (ClientSocket.size() < 3)
                {
                    myReporter.logs("New client.", "INFO");
                    int NewClient = accept(MasterSocket, 0, 0);
                    ClientSocket.push_back(NewClient);
                    set_nonblock(NewClient);
                    Event.data.fd = NewClient;
                    Event.events = EPOLLIN;
                    epoll_ctl(EPoll, EPOLL_CTL_ADD, NewClient, &Event);
                }
            } else {
                static char Buffer[10240];
                int RecvSize = recv(Events[i].data.fd, Buffer, 10240, MSG_NOSIGNAL);
                if ((RecvSize == 0) && (errno != EAGAIN))
                {
                    myReporter.logs("Client disconnected.", "INFO");
					epoll_ctl(EPoll, EPOLL_CTL_DEL, Events[i].data.fd, NULL);
                    shutdown(Events[i].data.fd, SHUT_RDWR);
                    ClientSocket.erase(find(ClientSocket.begin(), ClientSocket.end(), Events[i].data.fd));
                    close(Events[i].data.fd);
                }
                else if (RecvSize > 0)
                {
                    if (strncmp(Buffer, "quit", 4) == 0)
                    {
					    myReporter.logs("Request quitting.", "INFO");
                        close_server();
					}
                    else
                    {
					    std::string strBuffer(Buffer);
						myReporter.logs("User input : " + strBuffer.substr(0, strBuffer.find('\n')), "LOG");
					}
                }
            }
        }
    }
}

Matt_daemon::~Matt_daemon()
{
}
