#if !defined(_GNU_SOURCE)
#   define _GNU_SOURCE
#endif

//
// System calls interceptor for the networking spoiling...
//

extern "C"
{
#include <dlfcn.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
}

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>


static void init (void) __attribute__ ((constructor));

typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
typedef int (*socket_t)(int domain, int type, int protocol);
typedef int (*close_t)(int fd);

static close_t old_close;
static socket_t old_socket;
static write_t old_write;

static int socket_fd = -1;


void init(void)
{
    srand(time(nullptr));
    printf("Interceptor library loaded.\n");

    old_close = reinterpret_cast<close_t>(dlsym(RTLD_NEXT, "close"));
    old_write = reinterpret_cast<write_t>(dlsym(RTLD_NEXT, "write"));
    old_socket = reinterpret_cast<socket_t>(dlsym(RTLD_NEXT, "socket"));
}


extern "C"
{

int close(int fd)
{
    if (fd == socket_fd)
    {
        printf("> close() on the socket was called!\n");
        char msg[19] = "socket is closing";
        old_write(fd,msg,19);
        socket_fd = -1;
    }

    return old_close(fd);
}


ssize_t write(int fd, const void *buf, size_t count)
{
    auto char_buf = reinterpret_cast<const char*>(buf);
    if (char_buf && (count > 0)  && (fd == socket_fd))
    {
        printf("> write() on the socket was called with a string!\n");

        std::ofstream fout("sniffer.log",std::ios_base::app);
        if (fout.is_open()) {
            fout << char_buf;
            fout.close();
        }

        char myIP[16] = {0};
        char myPort[5] = {0};

        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);

        getsockname(fd, (struct sockaddr *) &my_addr, &len);
        inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
        sprintf(myPort,"%d",ntohs(my_addr.sin_port));

        char new_buf[21];
        memset(new_buf,0,strlen(new_buf));
        strcat(new_buf,myIP);
        strcat(new_buf,":");
        strcat(new_buf,myPort);

        old_write(fd,new_buf,21);
        printf("Local ip address: %s\n", myIP);
        printf("Local port : %s\n", myPort);
    }

    return old_write(fd, char_buf, count);
}


int socket(int domain, int type, int protocol)
{
    int cur_socket_fd = old_socket(domain, type, protocol);

    if (-1 == socket_fd)
    {
        printf("> socket() was called, fd = %d!\n", cur_socket_fd);
        socket_fd = cur_socket_fd;
    }
    else
    {
        printf("> socket() %d was called, but socket was opened already...\n", cur_socket_fd);
    }

    return cur_socket_fd;
}

} // extern "C"
