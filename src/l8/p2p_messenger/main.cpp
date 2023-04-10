#include <iostream>
#include <boost/asio.hpp>
#include "p2p.h"

int main(int argc, char* argv[])
{
    boost::asio::thread_pool thread_pool(2);

    if(argc != 3){
        std::cerr << "Usage: ./peer <your_nickname> <multicast_address>" << std::endl;
        std::exit(1);
    }

    const auto name = argv[1];
    const auto addr = argv[2];

    boost::asio::io_context io_context;
    boost::asio::ip::address chat_room(boost::asio::ip::make_address(addr));

    Peer peer(io_context, chat_room, name);

    boost::asio::post(thread_pool, [&]
    {
        peer.do_receive();
        io_context.run();
    });

    boost::asio::post(thread_pool, [&]
    {
        peer.do_send();
        io_context.run();
    });

    thread_pool.join();

    return EXIT_SUCCESS;
}
