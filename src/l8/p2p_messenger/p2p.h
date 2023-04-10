#include <iostream>
#include <string>
#include <array>
#include <random>

#include <boost/asio.hpp>

constexpr uint16_t multicast_port = 8082;

class Peer {
private:
    size_t maximum_message_size_ = 128;
    std::array<char, 128> receiving_buffer_;
    std::string name_;
    uint16_t uid_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::asio::ip::udp::endpoint multicast_endpoint_;

public:
    Peer(boost::asio::io_context& io_context,
             const boost::asio::ip::address& chat_room,
             const std::string& name)
            : socket_(io_context)
            , multicast_endpoint_(chat_room, multicast_port)
            , name_(name)
    {
        srand(time(0));
        uid_ = rand();

        boost::asio::ip::udp::endpoint listen_endpoint(chat_room, multicast_port);

        socket_.open(listen_endpoint.protocol());
        socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        socket_.set_option(boost::asio::ip::multicast::join_group(chat_room));

        auto welcome_message = std::string(name_ + " connected to the chat\n");

        socket_.async_send_to(boost::asio::buffer(welcome_message), multicast_endpoint_,
                            [this](const boost::system::error_code& error_code, std::size_t bytes_sent)
                            {
                                if (!error_code)
                                {
                                    std::cout << "Entered chat room successfully" << std::endl;
                                }
                            });
    }

    void do_receive()
    {
        socket_.async_receive_from(boost::asio::buffer(receiving_buffer_), remote_endpoint_,
                                [this](const boost::system::error_code& error_code, std::size_t bytes_received)
                                {
                                    if (!error_code.failed() && bytes_received > 0)
                                        {
                                            auto rcvd_message = std::string(receiving_buffer_.begin(), receiving_buffer_.begin() + bytes_received);

                                            if (rcvd_message.find(name_) != 0)
                                            {
                                                std::cout.write(receiving_buffer_.data(), bytes_received);
                                                std::cout << "\n" << remote_endpoint_ << "\n" << multicast_endpoint_ << std::flush;
                                            }
                                            do_receive();
                                        }
                                });
    };

    void do_send()
    {
        std::string name = name_;
        std::string message;

        std::getline(std::cin, message);
        std::string buffer = name.append(": " + message);

        socket_.async_send_to(boost::asio::buffer(buffer, maximum_message_size_), multicast_endpoint_,
                            [this, message](const boost::system::error_code& /*error_code*/, std::size_t bytes_sent)
                            {
                                std::cout << "You: " << message << std::endl;
                                do_send();
                            });
    };

    void send_info()
    {
        std::string info = std::to_string(uid_) + " " + name_;

        socket_.async_send_to(boost::asio::buffer(info, maximum_message_size_), multicast_endpoint_,
                            [this](const boost::system::error_code& /*error_code*/, std::size_t bytes_sent)
                            {
                                do_receive();
                            });
    };


};
