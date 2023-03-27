#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <list>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "file_struct.h"
#include "tcpserver.h"

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

#include <cerrno>

#if !defined(_WIN32)
extern "C"
{
#   include <signal.h>
}
#else
#   include <cwctype>
#endif

#if !defined(MAX_PATH)
#   define MAX_PATH (256)
#endif


const auto clients_count = 10;
using namespace std::literals;
namespace fs = std::filesystem;

#if defined(_WIN32)
const wchar_t separ = fs::path::preferred_separator;
#else
const wchar_t separ = *reinterpret_cast<const wchar_t*>(&fs::path::preferred_separator);
#endif

std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>
get_serv_info(const char *port)
{
    struct addrinfo hints =
    {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    };
    struct addrinfo *s_i;
    int ai_status;

    if ((ai_status = getaddrinfo(nullptr, port, &hints, &s_i)) != 0)
    {
        std::cerr << "getaddrinfo error " << gai_strerror(ai_status) << std::endl;
        return std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(nullptr, freeaddrinfo);
    }

    return std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(s_i, freeaddrinfo);
}


socket_wrapper::Socket accept_client(socket_wrapper::Socket &server_sock)
{
    struct sockaddr_storage client_addr;
    socklen_t client_addr_length = sizeof(client_addr);
    std::array<char, INET_ADDRSTRLEN> addr;

    socket_wrapper::Socket client_sock(accept(server_sock, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_length));

    if (!client_sock)
    {
        throw std::logic_error("Accepting client");
    }

    assert(sizeof(sockaddr_in) == client_addr_length);

    std::cout <<
        "Client from " << inet_ntop(AF_INET, &(reinterpret_cast<const sockaddr_in * const>(&client_addr)->sin_addr), &addr[0], addr.size())
        << "..."
        << std::endl;
    return client_sock;
}



int main(int argc, const char * const argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

#if !defined(_WIN32)
    signal(SIGPIPE, SIG_IGN);
#endif

    //socket_wrapper::SocketWrapper sock_wrap;

    try
    {
        auto servinfo = get_serv_info(argv[1]);
        if (!servinfo)
        {
            std::cerr << "Can't get servinfo!" << std::endl;
            exit(EXIT_FAILURE);
        }

        int port = std::stoi(argv[1]);

        boost::asio::io_context io_context;
        TcpServer server(io_context,port);
        io_context.run();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }


    return EXIT_SUCCESS;
}

