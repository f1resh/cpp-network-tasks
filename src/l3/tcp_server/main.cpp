#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <string.h>
#include "sys/socket.h"
#include <fstream>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base(),s.end());
    return s;
}


int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    socket_wrapper::SocketWrapper sock_wrap;
    const int port { std::stoi(argv[1]) };

    socket_wrapper::Socket sock = {AF_INET6, SOCK_STREAM, IPPROTO_TCP};

    std::cout << "Starting echo server on the port " << port << "...\n";

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in6 addr =
    {
        .sin6_family = AF_INET6,
        .sin6_port = htons(port),
    };

    addr.sin6_addr = in6addr_any;

    if (bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return EXIT_FAILURE;
    }

    listen(sock,1);
    int enable = 1;
    int rcv_buf = 256;
    int no = 0;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcv_buf, sizeof(int));
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));

    char buffer[256] = {0};

    // socket address used to store client address
    struct sockaddr_in6 client_address = {0};
    socklen_t client_address_len = sizeof(sockaddr_in6);
    ssize_t recv_len = 0;

    std::cout << "Running echo server...\n" << std::endl;
    char client_address_buf[INET6_ADDRSTRLEN];

    bool run = true;

    int newsockfd = accept(sock, reinterpret_cast<sockaddr*>(&client_address),&client_address_len);

    while (run)
    {


        // Read content into buffer from an incoming client.
        recv_len = recv(newsockfd, buffer, sizeof(buffer) - 1, 0);
        if (recv_len > 0)
        {
            buffer[recv_len] = '\0';
            std::cout
                << "Client with address "
                << inet_ntop(AF_INET6, &client_address.sin6_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
                << ":" << ntohs(client_address.sin6_port)
                << " sent datagram "
                << "[length = "
                << recv_len
                << "]:\n'''\n"
                << buffer
                << "\n'''"
                << std::endl;



            std::string command_string = {buffer};
            rtrim(command_string);

            if ("exit" == command_string) run = false;

            //recieve request to download file
            if (command_string.rfind(("file ",0) == 0)){
                std::fstream file;
                std::string filepath;
                std::stringstream ss(command_string);
                ss >> filepath >> filepath; //get second word after space - filename

                file.open(filepath, std::fstream::in | std::fstream::binary);
                if(file.is_open()){
                   std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                   std::cout << "Sending file: " << filepath << std::endl;
                   int bytes_sent = send(newsockfd , contents.c_str() , contents.length() , 0 );

                }
                else{
                   std::cout<<"[ERROR] : File loading failed.\n";
                   send(newsockfd, "File not found!\0", 16, 0);
                }

            }
            else{

                //convert client address to readable
                char client_addr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET6,&(client_address.sin6_addr),client_addr,INET6_ADDRSTRLEN);

                char reply[356];
                memset(reply, 0, sizeof(reply));

                strcat(reply,"(");
                strcat(reply,client_addr);
                strcat(reply,"): ");
                strcat(reply,buffer);

                // Send same content back to the client ("echo").
                send(newsockfd, reply, sizeof(reply), 0);
            }

        }
    }
    close(sock);
    return EXIT_SUCCESS;
}

