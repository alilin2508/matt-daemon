#include "Matt_daemon.hpp"
#include "Tintin_reporter.hpp"

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
		unlockDaemon();
    close(MasterSocket);
    close(EPoll);
    exit(EXIT_SUCCESS);
}

void	Matt_daemon::unlockDaemon() {
	std::remove("/var/lock/matt_daemon.lock");	
}

static void Matt_daemon::signalHandler(int sig) {
		unlockDaemon();
    cout << "Signal received: " << sig << endl;
    exit(EXIT_SUCCESS);
}

Matt_daemon::Matt_daemon()
{
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
    snprintf(daemonPidStr, sizeof(daemonPidStr), "%s%d", "started. PID: ",daemonPid);

    umask(0);
		
	myReporter.openOrCreate("/var/log/matt_daemon");
	myReporter.logs("Entering Daemon mode.", "INFO");
	myReporter.logs(daemonPidStr, "INFO");
	myReporter.closeStream();

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
                    cout << "New client" << endl;
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
                if ((RecvSize == 0) && (errno != EAGAIN)) {
                    cout << "Client disconnected" << endl;
					epoll_ctl(EPoll, EPOLL_CTL_DEL, Events[i].data.fd, NULL);
                    shutdown(Events[i].data.fd, SHUT_RDWR);
                    ClientSocket.erase(find(ClientSocket.begin(), ClientSocket.end(), Events[i].data.fd));
                    close(Events[i].data.fd);
                } else if (RecvSize > 0) {
                    if (strcmp(Buffer, "quit\n") == 0)
                        close_server();
                    //print in logs
                }
            }
        }
    }
}

Matt_daemon::~Matt_daemon()
{
  std::cout << "Destructor Matt Daemon called" << std::endl;
}
