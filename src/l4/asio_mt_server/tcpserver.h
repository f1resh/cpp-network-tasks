#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "file_struct.h"
#include <vector>
#include <filesystem>
#include <fstream>



// Указатель shared_ptr и enable_shared_from_this нужны для того,
// чтобы сохранить объект tcp_connection до завершения выполнения операции.
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
    boost::asio::ip::tcp::socket socket_;
    std::string message_;
    const int buffer_size = 4096;
    std::vector<char> buff_;
    std::ifstream source_file;

public:
    typedef std::shared_ptr<TcpConnection> pointer;
    static pointer create(boost::asio::io_context& io_context)
    {
        return pointer(new TcpConnection(io_context));
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return socket_;
    }
    // В методе start(), вызывается asio::async_write(), отправляющий данные клиенту.
    // Здесь используется asio::async_write(), вместо ip::tcp::socket::async_write_some(), чтобы весь блок данных был гарантированно отправлен.
    void start()
    {
        // The data to be sent is stored in the class member message_ as we need to keep the data valid until the asynchronous operation is complete.

        // listen_for_request
        // parse request
        // answer request
        auto s = shared_from_this();

        socket_.async_read_some(boost::asio::buffer(buff_, buff_.size()), [s] (const boost::system::error_code& error, size_t bytes_transferred)
            {
                    if (!error){
                        // handle_write() выполнит обработку запроса клиента.
                        file_struct fstruct(s->buff_.data());
                        std::cout << "Sending file " << fstruct.filepath << std::endl;
                        s->source_file.open(fstruct.filepath, std::ios_base::binary);
                        if (!(s->source_file))
                        {
                            std::cout << "failed to open " << fstruct.filepath << std::endl;
                            return ;
                        }
                        s->doWriteFile(error,bytes_transferred);
                        //s->handle_write(error, bytes_transferred);
                    }
            }
        );
    }

private:
    TcpConnection(boost::asio::io_context& io_context)
    : socket_(io_context)
    {
        buff_.resize(buffer_size);
    }
    void handle_write(const boost::system::error_code& /*error*/, size_t bytes_transferred)
    {
        std::cout << "Bytes transferred: " << bytes_transferred << std::endl;
    }

    uint64_t fileSize(std::filesystem::path const& file_path){

        return std::filesystem::file_size(file_path);
    }

    void writeBuffer(std::vector<char>& buff_)
    {
        boost::asio::async_write(socket_,
            boost::asio::buffer(buff_.data(), static_cast<size_t>(source_file.gcount())),
            [this](boost::system::error_code ec, size_t bytes_transferred)
            {
                doWriteFile(ec,bytes_transferred);
            });
    }

    void doWriteFile(const boost::system::error_code& t_ec, size_t bytes_transferred)
    {
        if (!t_ec) {
            if (source_file) {
                source_file.read(&buff_[0], buff_.size());
                if (source_file.fail() && !source_file.eof()) {
                    auto msg = "Failed while reading file";
                    throw std::fstream::failure(msg);
                }
                std::stringstream ss;
                ss << "Send " << source_file.gcount() << " bytes, total: "
                    << source_file.tellg() << " bytes";
                std::cout << ss.str() << std::endl;
                writeBuffer(buff_);
            }
        }
    }

};




class TcpServer
{
private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;

public:
    TcpServer(boost::asio::io_context& io_context, int port) :
        io_context_(io_context),
        acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)){
        start_accept();
    };
private:
// Метод start_accept() создаёт сокет и выполняет асинхронный `accept()`,при соединении.
    void start_accept()
    {
        TcpConnection::pointer new_connection =
        TcpConnection::create(io_context_);
        acceptor_.async_accept(new_connection->socket(),
        [this, new_connection] (const boost::system::error_code& error)
            {
                std::cout << "New client connected: " << new_connection->socket().remote_endpoint().address().to_string()
                          << " on port " << new_connection->socket().remote_endpoint().port() << std::endl;
                this->handle_accept(new_connection, error);
            }
        );
    }
    // Метод handle_accept() вызывается, когда асинхронный accept, инициированный в start_accept() завершается.
    // Она выполняет обработку запроса клиента и запуск нового акцептора.
    void handle_accept(TcpConnection::pointer new_connection, const
        boost::system::error_code& error)
        {
            if (!error)
            {
                new_connection->start();
            }
            else
            {
            }
        start_accept();
        }

};


//class FileHandler
//{

//public:
//    bool send_buffer(const std::vector<char> &buffer)
//    {
//        size_t transmit_bytes_count = 0;
//        const auto size = buffer.size();
//        //std::cout << size << std::endl;

//        while (transmit_bytes_count != size)
//        {
//            auto result = send(client_sock_, &(buffer.data()[0]) + transmit_bytes_count, size - transmit_bytes_count, 0);
//            if (-1 == result)
//            {
//                if (need_to_repeat()) continue;
//                return false;
//            }

//            transmit_bytes_count += result;
//        }

//        return true;
//    }

//    void send_msg(const std::string& msg){
//        send(client_sock_, msg.c_str(), msg.length(), 0);
//    }

//    bool send_file(const file_struct& fstruct)
//    {
//        std::vector<char> buffer(buffer_size);
//        std::ifstream file_stream(fstruct.filepath, std::ifstream::binary);
//        uint64_t filesize = fileSize(fstruct.filepath);

//        if (fstruct.offset == -1 || fstruct.length == -1) return false;

//        int tmp_length = fstruct.length;

//        if (!file_stream) return false;

//        if (fstruct.offset+fstruct.length > filesize){
//            std::cout << "Requested offset + length exceed filesize!" << std::endl;
//            return false;
//        }

//        file_stream.seekg(fstruct.offset, std::ios::beg);


//        while (file_stream && tmp_length)
//        {
//            if (tmp_length >= buffer.size()){
//                file_stream.read(&buffer[0], buffer.size());
//                tmp_length -= buffer.size();
//            }else{
//                buffer.resize(tmp_length);
//                file_stream.read(&buffer[0], tmp_length);
//                tmp_length = 0;
//            }

//            if (!send_buffer(buffer)) return false;
//        }

//        return true;
//    }



//class Client
//{
//public:
//    Client(socket_wrapper::Socket &&sock) :
//        tsr_(std::move(sock))
//    {
//        std::cout
//            << "Client ["<< static_cast<int>(tsr_.ts_socket()) << "] "
//            << "was created..."
//            << std::endl;
//    }

//    std::optional<file_struct> recv_file_path()
//    {
//        auto request_data = tsr_.get_request();
//        if (!request_data.filepath.size()) return std::nullopt;

//        auto cur_path = fs::current_path().wstring();
//        auto file_path = fs::weakly_canonical(request_data.filepath).wstring();

//#if defined(_WIN32)
//        std::transform(cur_path.begin(), cur_path.end(), cur_path.begin(),
//            [](wchar_t c) { return std::towlower(c); }
//        );
//        std::transform(file_path.begin(), file_path.end(), file_path.begin(),
//            [](wchar_t c) { return std::towlower(c); }
//        );
//#endif
//        if (file_path.find(cur_path) == 0)
//        {
//            file_path = file_path.substr(cur_path.length());
//        }

//        return request_data; //fs::weakly_canonical(cur_path + separ + file_path);
//    }

//    bool send_file(const file_struct& fstruct)
//    {

//        if (!(fs::exists(fstruct.filepath) && fs::is_regular_file(fstruct.filepath))) return false;

//        return tsr_.send_file(fstruct);
//    }

//    bool process()
//    {
//        auto file_to_send = recv_file_path();
//        bool result = false;

//        if (std::nullopt != file_to_send)
//        {
//            std::cout << "Trying to send " << file_to_send->filepath << "..." << std::endl;
//            if (send_file(*file_to_send))
//            {
//                std::cout << "File was sent." << std::endl;
//            }
//            else
//            {
//                std::cerr << "File sending error!" << std::endl;
//                tsr_.send_msg("-1");
//                return false;
//            }
//            result = true;
//        }

//        return result;
//    }

//private:
//    Transceiver tsr_;
//    fs::path file_path_;
//};
