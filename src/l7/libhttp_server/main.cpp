#include <iostream>
#include <memory>
#include <string>

#include <httpserver.hpp>
using namespace httpserver;

#include "auth.h"

bool run = true;

std::shared_ptr<http_response> not_found_custom(const http_request& req) {
    return std::shared_ptr<string_response>(new string_response("Not found resource", 404, "text/plain"));
}

std::shared_ptr<http_response> not_allowed_custom(const http_request& req) {
    return std::shared_ptr<string_response>(new string_response("Not allowed method!", 405, "text/plain"));
}

bool isAuthorised(const http_request& req){
    return req.get_header("auth")=="yes";
}

std::string get_file_path(std::string& original_str){
    size_t pos = original_str.find("/file");
    return original_str.substr(pos+6, original_str.length()-1);
}
//------------------------------------------------------------------------------
// RESOURCES

class login_resource : public http_resource {
    public:
        std::shared_ptr<http_response> render_POST(const http_request& req) {
            PasswordFileReader pReader;
            AuthPassword authPwd(pReader.createMap());
            std::string body{req.get_content()};
            if (authPwd.isValid(body)){
                //here we can send the jwt or cookie back
                return std::shared_ptr<http_response>(new string_response("yes",200,"text/plain"));
            };
            return std::shared_ptr<http_response>(new string_response("Authorization failed!",401,"text/plain"));
        }

};

class exit_resource : public http_resource {
    public:
        std::shared_ptr<http_response> render_POST(const http_request& req) {
            if (isAuthorised(req)){
                run = false;
                return std::shared_ptr<http_response>(new string_response("Server is shutting down!",200,"text/plain"));
            }
            return std::shared_ptr<http_response>(new string_response("Not authorized!",401,"text/plain"));
        }

    };

class download_resource : public http_resource {
    public:
        std::shared_ptr<http_response> render_GET(const http_request& req) {
            if (isAuthorised(req)){
                std::string path(req.get_path());
                path = get_file_path(path);
                if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)){
                    return std::shared_ptr<file_response>(new file_response(path,200,"text/plain"));
                } else{
                    return std::shared_ptr<http_response>(new string_response("File not found!",404,"text/plain"));
                }

            }
            return std::shared_ptr<http_response>(new string_response("Not authorized!",401,"text/plain"));

        }
    };
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc != 3)
    {
        std::cerr <<
            "Usage: libhttp-server <port> <threads>\n" <<
            "Example:\n" <<
            "    libhttp-server 8080\n";
        return EXIT_FAILURE;
    }
    auto const port = static_cast<unsigned short>(std::atoi(argv[1]));

    webserver ws = create_webserver(port)
            .use_dual_stack()
            .not_found_resource(not_found_custom)
            .method_not_allowed_resource(not_allowed_custom)
            .use_ssl()
            .https_mem_key("key.pem")
            .https_mem_cert("server.pem");

    //Resources
    login_resource login_res;
    login_res.disallow_all();
    login_res.set_allowing("POST", true);

    exit_resource exit_res;
    exit_res.disallow_all();
    exit_res.set_allowing("POST", true);

    download_resource download_res;
    download_res.disallow_all();
    download_res.set_allowing("GET", true);

    ws.register_resource("/login",&login_res);
    ws.register_resource("/server/exit",&exit_res);
    ws.register_resource("/file",&download_res, true);

    ws.start(false);
    while (run){
        sleep(1);

    }
    std::cout << "Stopping server!" << std::endl;
    ws.stop();


    return EXIT_SUCCESS;
}
