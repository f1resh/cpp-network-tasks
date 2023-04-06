#include <cpr/cpr.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <boost/algorithm/string.hpp>


// Performs an HTTP GET and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 6 && argc != 7)
        {
            std::cerr <<
                "Usage: libcurl-client <host> <port> <method> <target> <login:password>[<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    libcurl-client www.example.com 80 POST / user:password\n" <<
                "    libcurl-client www.example.com 80 GET / user:password 1.0\n";
            return EXIT_FAILURE;
        }

        std::string const host = argv[1];
        std::string const port = argv[2];
        std::string method = argv[3];
        std::string const target = argv[4];
        std::string const creds = argv[5];
        int version = argc == 7 && !std::strcmp("1.0", argv[6]) ? 10 : 11;

        boost::algorithm::to_lower(method);

        if (method != "post" && method != "get" && method != "head")
        {
            std::cerr << "Error: please provide HTTP method: POST or GET or HEAD" << std::endl;
            return EXIT_FAILURE;
        }



        // The SSL
        //cpr::SslOptions sslOpts = cpr::Ssl(ssl::TLSv1_2{});

        //AUTH
        cpr::Response response = cpr::Post(cpr::Url{"https://"+host+":"+port+"/login"},cpr::Body{creds},cpr::Header{{"Content-Type","text/plain"}},cpr::VerifySsl(0));

        //exit if not authorised
        if (response.status_code != 200 && response.text != "yes"){
            std::cerr << "Authentication error! Closing connection and exit." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (method == "post"){
            response = cpr::Post(cpr::Url{"https://"+host+":"+port+target},cpr::Header{{"auth","yes"}},cpr::VerifySsl(0));
        }else if (method == "get"){
            response = cpr::Get(cpr::Url{"https://"+host+":"+port+target},cpr::Header{{"auth","yes"}},cpr::VerifySsl(0));
        }else if (method == "head"){
            response = cpr::Head(cpr::Url{"https://"+host+":"+port+target},cpr::Header{{"auth","yes"}},cpr::VerifySsl(0));
        }

        if (response.status_code == 200){
            std::cout << "Downloading file..." << std::endl;
            std::fstream file;
            file.open("downloaded_file.txt", std::fstream::out | std::fstream::binary);
            if(!file.is_open()){
                std::cout<<"[ERROR] : File creation failed, Exiting.\n";
                exit(EXIT_FAILURE);
            }
            file << (response.text);
            file.close();

        }else{
        // Write the message to standard out
        std::cout << "Response code: " << response.status_code << ". Response body: " <<  response.text << std::endl;
        }

    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
