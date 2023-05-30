#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "root_certificates.hpp"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;   // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

// Performs an HTTP GET and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 6 && argc != 7)
        {
            std::cerr <<
                "Usage: beast-ssl-client <host> <port> <method> <target> <login:password>[<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    beast-ssl-client www.example.com 80 POST user:password /\n" <<
                "    beast-ssl-client www.example.com 80 GET user:password / 1.0\n";
            return EXIT_FAILURE;
        }

        auto const host = argv[1];
        auto const port = argv[2];
        auto const method = argv[3];
        auto const target = argv[4];
        auto const creds = argv[5];
        int version = argc == 7 && !std::strcmp("1.0", argv[6]) ? 10 : 11;

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

        // The SSL context is required, and holds certificates
        ssl::context ctx(ssl::context::tlsv12_client);

        // This holds the root certificate used for verification
        load_root_certificates(ctx);

        // Verify the remote server's certificate
        //ctx.set_verify_mode(ssl::verify_peer);

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(!SSL_set_tlsext_host_name(stream.native_handle(), host))
        {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{ec};
        }

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(stream).connect(results);

        // Perform the SSL handshake
        stream.handshake(ssl::stream_base::client);
        beast::error_code ec;

        //AUTH
        http::request<http::string_body> auth{http::verb::post, "/login", version};
        auth.set(http::field::host, host);
        auth.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        auth.body() = creds;
        auth.prepare_payload();

        //send auth
        http::write(stream, auth);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        //exit if not authorised
        if (res.base().result() != http::status::ok && boost::beast::buffers_to_string(res.body().data()) != "yes"){
            std::cout << boost::beast::buffers_to_string(res.body().data()) << std::endl;
            std::cerr << "Authentication error! Closing connection and exit." << std::endl;
            stream.shutdown(ec);
            exit(EXIT_FAILURE);
        }
        // Set up an HTTP request message
        http::request<http::string_body> req{method_verb, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        //simple auth flag
        req.set("auth","yes");

        // Send the HTTP request to the remote host
        http::write(stream, req);

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

        // Gracefully close the stream

        stream.shutdown(ec);
        if(ec == net::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec = {};
        }
        if(ec)
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
