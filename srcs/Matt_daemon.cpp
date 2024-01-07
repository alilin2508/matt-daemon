#include "Matt_daemon.hpp"
#include "Tintin_reporter.hpp"

int Matt_daemon::set_nonblock(int fd)
{
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Matt_daemon::~Matt_daemon(){
	std::cout << "Destructor Matt Daemon called" << std::endl;
}

Matt_daemon::Matt_daemon()
{
		signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

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

//		while(1);
	
    int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(PORT);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));

    set_nonblock(MasterSocket);

    listen(MasterSocket, SOMAXCONN);

    int EPoll = epoll_create1(0);

    struct epoll_event Event;
    Event.data.fd = MasterSocket;
    Event.events = EPOLLIN;
    epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);

    while (1) {
        struct epoll_event Events[MAX_EVENTS];
        int N = epoll_wait(EPoll, Events, MAX_EVENTS, -1);

        for (unsigned int i = 0; i < N; ++i) {
            if (Events[i].data.fd == MasterSocket) {
                int SlaveSocket = accept(MasterSocket, 0, 0);
                set_nonblock(SlaveSocket);
                struct epoll_event Event;
                Event.data.fd = SlaveSocket;
                Event.events = EPOLLIN;
                epoll_ctl(EPoll, EPOLL_CTL_ADD, SlaveSocket, &Event);
            } else {
                static char Buffer[10240];
                int RecvSize = recv(Events[i].data.fd, Buffer, 10240, MSG_NOSIGNAL);
                if ((RecvSize == 0) && (errno != EAGAIN)) {
                    shutdown(Events[i].data.fd, SHUT_RDWR);
					epoll_ctl(EPoll, EPOLL_CTL_DEL, Event[i].data.fd, NULL);
                    close(Events[i].data.fd);
                } else if (RecvSize > 0) {
                    send(Events[i].data.fd, Buffer, RecvSize, MSG_NOSIGNAL);
                }
            }
        }
    }
    //return 0;
}
