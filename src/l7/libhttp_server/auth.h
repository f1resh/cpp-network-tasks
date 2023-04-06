#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <tuple>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <boost/compute/detail/sha1.hpp>

using pmap = std::unordered_map<std::string,std::string>;
using pair = std::pair<std::string,std::string>;

//using fs = std:::filesystem;
const std::string logins_path = "logins.txt";

const std::string delimiter = ":";
pair parseLine(const std::string& line){
    pair result;
    int delim_pos = line.find(delimiter);
    std::string login = line.substr(0,delim_pos);
    std::string password = line.substr(delim_pos+1,line.length()-delim_pos);
    result.first = login;
    result.second = boost::trim_right_copy(password);
    return result;
}

class PasswordFileReader{
private:
    std::ifstream file;
public:
    PasswordFileReader(){
        openFile(logins_path);
    }
//    PasswordFileReader(const std::string& path){
//        openFile(path);
//    }

    ~PasswordFileReader(){
        file.close();
    }

    pmap* createMap(){
        pmap* result_map = new pmap();
        std::string line;
        while(std::getline(file,line)){
            result_map->insert(parseLine(line));
        }
        return result_map;
    }
private:
    void openFile(const std::string& path){
        if (!std::filesystem::exists(std::filesystem::path{path}))
            throw std::invalid_argument("Passwords file can't be found!");
        file.open(path);
    }


};

class AuthPassword{
private:
    pmap* login_map_;
public:
    AuthPassword(pmap* login_map):login_map_(std::move(login_map)){}
    ~AuthPassword(){delete login_map_;}

    bool isValid(const std::string& login, const std::string& password){
        if (login_map_->count(login) != 0){
            boost::compute::detail::sha1 sha1{password};
            if (login_map_->at(login).compare(std::string{sha1}) == 0)
                return true;
        }
        return false;
    }

    bool isValid(const std::string& line){
        pair temp = parseLine(line);
        if (login_map_->count(temp.first) != 0){
            boost::compute::detail::sha1 sha1{temp.second};
            if (login_map_->at(temp.first).compare(std::string{sha1}) == 0)
                return true;
        }
        return false;
    }
};

