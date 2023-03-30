#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

// Performs an HTTP GET and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 5 && argc != 6)
        {
            std::cerr <<
                "Usage: http-client-sync <host> <port> <method> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    beast-tcp-client www.example.com 80 POST /\n" <<
                "    beast-tcp-client www.example.com 80 GET / 1.0\n";
            return EXIT_FAILURE;
        }

        auto const host = argv[1];
        auto const port = argv[2];
        auto const method = argv[3];
        auto const target = argv[4];
        int version = argc == 6 && !std::strcmp("1.0", argv[5]) ? 10 : 11;

        http::verb method_verb;
        if (method == std::string("POST")) method_verb = http::verb::post;
        else if (method == std::string("GET")) method_verb = http::verb::get;
        else if (method == std::string("HEAD")) method_verb = http::verb::head;
        else {
            std::cerr << "Error: please provide HTTP method: POST or GET" << std::endl;
            return EXIT_FAILURE;
        }

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);

        // Set up an HTTP GET request message
        http::request<http::string_body> req{method_verb, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);


        if (res.base().result() == http::status::ok){
            std::cout << "Downloading file..." << std::endl;
            std::fstream file;
            file.open("downloaded_file.txt", std::fstream::out | std::fstream::binary);
            if(!file.is_open()){
                std::cout<<"[ERROR] : File creation failed, Exiting.\n";
                exit(EXIT_FAILURE);
            }
            file << boost::beast::buffers_to_string(res.body().data());
            file.close();

        }else{
        // Write the message to standard out
        std::cout << res << std::endl;
        }

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
