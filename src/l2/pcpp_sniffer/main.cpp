#include <string>
#include <iostream>
#include "stdlib.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "IPv4Layer.h"

const auto CAPTURE_PACKET_TIME = 10;
static int ip_counter = 0;

std::string getProtocolTypeAsString(pcpp::ProtocolType protocolType)
{
    switch (protocolType)
    {
    case pcpp::Ethernet:
        return "Ethernet";
    case pcpp::IPv4:
        return "IPv4";
    case pcpp::TCP:
        return "TCP";
    case pcpp::HTTPRequest:
    case pcpp::HTTPResponse:
        return "HTTP";
    default:
        return "Unknown";
    }
}

bool parseIPv4(pcpp::Packet& packet){
    pcpp::IPv4Layer* ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
    if (ipLayer == NULL)
    {
        std::cerr << "Something went wrong, couldn't find IPv4 layer" << std::endl;
        return false;
    }

    std::cout << std::endl
        << "Packet number " << ip_counter++ << std::endl
        << "Source IP address: " << ipLayer->getSrcIPAddress() << std::endl
        << "Destination IP address: " << ipLayer->getDstIPAddress() << std::endl
        << "IP ID: 0x" << std::hex << pcpp::netToHost16(ipLayer->getIPv4Header()->ipId) << std::endl
        << "TTL: " << std::dec << (int)ipLayer->getIPv4Header()->timeToLive << std::endl <<std::endl;
    return true;

}

/**
* a callback function for the blocking mode capture which is called each time a packet is captured
*/
static bool onPacketArrivesBlockingMode(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* device, void* cookie)
{
    // parsed the raw packet
    pcpp::Packet parsedPacket(packet);
    parseIPv4(parsedPacket);


    // return false means we don't want to stop capturing after this callback
    return false;
}

int main(int argc, const char * const argv[])
{
    std::string dev;
    pcpp::PcapLiveDevice* device;

    // Check for capture device name on command-line.
    if (2 == argc)
    {
        dev = argv[1];
        device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(dev);

        if (device == NULL)
        {
            std::cerr << "Cannot find interface for device name '" << dev << "'" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "error: unrecognized command-line options\n" << std::endl;
        std::cout
            << "Usage: " << argv[0] << " [interface]\n\n"
            << "Options:\n"
            << "    interface    Listen on <interface> for packets.\n"
            << std::endl;

        exit(EXIT_FAILURE);
    }

    std::cout
        << "Interface info:" << std::endl
        << "   Interface name:        " << device->getName() << std::endl // get interface name
        << "   Interface description: " << device->getDesc() << std::endl // get interface description
        << "   MAC address:           " << device->getMacAddress() << std::endl // get interface MAC address
        << "   Default gateway:       " << device->getDefaultGateway() << std::endl // get default gateway
        << "   Interface MTU:         " << device->getMtu() << std::endl; // get interface MTU

    if (device->getDnsServers().size() > 0)
        std::cout << "   DNS server:            " << device->getDnsServers().at(0) << std::endl << std::endl;

    // Open capture device.
    if (!device->open())
    {
        std::cerr << "Cannot open device" << std::endl;
        exit(EXIT_FAILURE);
    }

    void* empty;
    device->startCaptureBlockingMode(onPacketArrivesBlockingMode,&empty,CAPTURE_PACKET_TIME);

    std::cout << "\nTotal IPv4 packets captured: " << ip_counter << std::endl;
    std::cout << "Capture complete." << std::endl;

    return EXIT_SUCCESS;
}
