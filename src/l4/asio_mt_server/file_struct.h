#pragma once
#include <string>


struct file_struct{

public:
    std::string filepath;
    int offset;
    int length;

public:
    file_struct(std::string rcvd_msg){
        parse_string(rcvd_msg);
    };

private:
    void parse_string(std::string rcvd_msg){
        std::stringstream ss(rcvd_msg);
        std::string offset_s, length_s;
        ss >> filepath >> offset_s >> length_s;
        offset = parse_to_int(offset_s);
        length = parse_to_int(length_s);
    };

    int parse_to_int(std::string str){
        int result;
        try {
            result = stoi(str);
        }  catch (std::exception const &ex) {
            result = -1;
        }
        return result;
    }
};

