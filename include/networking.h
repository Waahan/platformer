#pragma once

#include <string>
#include <unordered_map>

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

    //Unix socket representations
    typedef int socketType;
    typedef socklen_t socketLengthType;
    typedef in_port_t portType;
#endif
}

namespace networking
{
    enum class ipVersion : int { ipv4 = AF_INET, ipv6 = AF_INET6 };
    enum class protocal : int { tcp = SOCK_STREAM, udp = SOCK_DGRAM };

    class networkingConfig
    {
        public:
        networkingConfig() = default;
        networkingConfig(struct sockaddr&& convert);

        networkingConfig(const networkingConfig& copyFrom) = default;
        networkingConfig& operator=(const networkingConfig& copyFrom) = default;

        networkingConfig(networkingConfig&& moveFrom) = default;
        networkingConfig& operator=(networkingConfig&& moveFrom) = default;

        ~networkingConfig() = default;

        ipVersion getIpVersion() const;
        std::string getIpAddress() const;
        portType getPort() const;

        networkingConfig& setIpVersion(ipVersion setIpVersion);
        networkingConfig& setIpAddress(const std::string& ipAddress);
        networkingConfig& setPort(portType portNumber);

        struct sockaddr* sockaddrRep();
        const struct sockaddr* sockaddrRep() const;

        size_t size();

        private:
        ipVersion currentIpVersion;

        union
        {
            sockaddr_in socketAddressIpv4; 
            sockaddr_in6 socketAddressIpv6;
        };
    };
} //Networking
