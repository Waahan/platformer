#pragma once

#include <string>

extern "C"
{
#ifdef __WINDOWS__
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>

    //Windows socket representations
    typedef SOCKET socketType;
    typedef int socketLengthType;
    typedef unsigned short portType;
#else
    #include <sys/socket.h>
    #include <sys/types.h> 
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>

    //Unix socket representations
    typedef int socketType;
    typedef socklen_t socketLengthType;
    typedef in_port_t portType;
#endif
}

namespace networking
{
    enum class protocal : int {tcp = SOCK_STREAM, udp = SOCK_DGRAM};
    enum class ipVersion : int {ipv4 = AF_INET, ipv6 = AF_INET6};

    /*
        A class that combines sockaddr_in and sockaddr_in6 

        Note that the does not check and mantain invariants 
    */
    class networkingConfig
    {
        public:
        networkingConfig() : socketAddress{} {}
        networkingConfig(struct sockaddr&& convert);

        networkingConfig(const networkingConfig& copyFrom) = default;
        networkingConfig& operator=(const networkingConfig& copyFrom) = default;

        networkingConfig(networkingConfig&& moveFrom) = default;
        networkingConfig& operator=(networkingConfig&& moveFrom) = default;

        ~networkingConfig() = default;

        ipVersion getIpVersion() const;
        std::string getIpAddress() const;
        portType getPort() const;

        networkingConfig& setIpVersion(ipVersion version);
        networkingConfig& setIpAddress(const std::string& ipAddress);
        networkingConfig& setPort(portType port);

        const struct sockaddr* sockaddrRep() const;

        private:
        union sockaddrUnion
        {
            struct sockaddr_in ipv4SocketAddress;
            struct sockaddr_in6 ipv6SocketAddress;
        };

        ipVersion currentVersion;
        sockaddrUnion socketAddress;
    };

    class server
    {
    };

    class client
    {
    };
} //networking
