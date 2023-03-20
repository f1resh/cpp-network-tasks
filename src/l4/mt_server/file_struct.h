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
        std::string word;
        ss >> filepath >> offset >> length;
    };
};

