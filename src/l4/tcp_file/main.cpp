#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#   define ioctl ioctlsocket
#else
extern "C"
{
#   include <netinet/tcp.h>
#   include <sys/ioctl.h>
// #   include <fcntl.h>
}
#endif

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


using namespace std::chrono_literals;

const auto MAX_RECV_BUFFER_SIZE = 4096;


bool send_request(socket_wrapper::Socket &sock, const std::string &request);
int open_socket(const char *hostname, const char *port);


int main(int argc, const char * const argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket sock = {AF_INET6, SOCK_STREAM, IPPROTO_TCP};

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sock = open_socket(argv[1],argv[2]);

    const std::string host_name = { argv[1] };

    std::string request;
    std::vector<char> buffer;
    buffer.resize(MAX_RECV_BUFFER_SIZE);

    std::cout << "Connected to \"" << host_name << "\"..." << std::endl;

    const IoctlType flag = 1;

    if (ioctl(sock, FIONBIO, const_cast<IoctlType*>(&flag)) < 0)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    // Disable Naggles's algorithm.
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&flag), sizeof(flag)) < 0)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Waiting for the user input..." << std::endl;

    while (true)
    {
        //request = "file ";
        std::string filepath;

        std::cout << "> " << std::flush;
        //std::cin >> tmp.filepath >> tmp.offset >> tmp.length;
        if (!std::getline(std::cin, filepath)) break;

        request = filepath;

        std::cout
            << "Sending request: \"" << request << "\"..."
            << std::endl;

        if (!send_request(sock, request))
        {
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return EXIT_FAILURE;
        }

        std::fstream file;
        file.open("downloaded_file.txt", std::fstream::out | std::fstream::binary);
        if(file.is_open()){
            std::cout<<"[LOG] : File Created.\n";
        }else{
            std::cout<<"[ERROR] : File creation failed, Exiting.\n";
            exit(EXIT_FAILURE);
        }

//        std::cout
//            << "Request was sent, reading response..."
//            << std::endl;

        std::this_thread::sleep_for(2ms);

        while (true)
        {
            auto recv_bytes = recv(sock, buffer.data(), buffer.size() - 1, 0);

            std::cout
                << recv_bytes
                << " was received..."
                << std::endl;

            if (recv_bytes > 0)
            {
                buffer[recv_bytes] = '\0';
                std::string str(buffer.data());

                if (str != "-1"){
                    file << buffer.data();

                }else{
                    std::cout << "File was not found on server!" << std::endl;
                    return EXIT_FAILURE;
                }
                continue;
            }
            else if (-1 == recv_bytes)
            {
                if (EINTR == errno) continue;
                if (0 == errno) break;
                // std::cerr << errno << ": " << sock_wrap.get_last_error_string() << std::endl;
                break;
            }

            break;
        }

        file.close();

    }

    return EXIT_SUCCESS;
}

int open_socket(const char *hostname, const char* port){
    int sd, err;

    struct addrinfo hints = {};
    struct addrinfo *result;

    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = IPPROTO_TCP;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    err = getaddrinfo(hostname, port, &hints, &result);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(err));
        abort();
    }

    for(struct addrinfo *addr = result; addr != NULL; addr = addr->ai_next)
    {
        sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sd == -1)
        {
            err = errno;
            continue;
        }

        if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        err = errno;

        close(sd);
        sd = -1;
        close(sd);
    }

    freeaddrinfo(result);

    if (sd == -1)
    {
        std::cout << "Unable to connect to hostname: " << hostname << std::endl;
        exit(EXIT_FAILURE);
    }

    return sd;
};

bool send_request(socket_wrapper::Socket &sock, const std::string &request)
{
    ssize_t bytes_count = 0;
    size_t req_pos = 0;
    auto const req_buffer = &(request.c_str()[0]);
    auto const req_length = request.length();

    while (true)
    {
        if ((bytes_count = send(sock, req_buffer + req_pos, req_length - req_pos, 0)) < 0)
        {
            if (EINTR == errno) continue;
        }
        else
        {
            if (!bytes_count) break;

            req_pos += bytes_count;

            if (req_pos >= req_length)
            {
                break;
            }
        }
    }

    return true;
}
