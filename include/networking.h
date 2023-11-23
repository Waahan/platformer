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
        /*
            Use set functions to initalize what you need
        */
        networkingConfig() : socketAddress{} {}
        
        /*
            Convert ipv4 and ipv6 sockaddr to networkConfig
        */
        networkingConfig(struct sockaddr&& convert);

        networkingConfig(const networkingConfig& copyFrom) = default;
        networkingConfig& operator=(const networkingConfig& copyFrom) = default;

        networkingConfig(networkingConfig&& moveFrom) = default;
        networkingConfig& operator=(networkingConfig&& moveFrom) = default;

        ~networkingConfig() = default;

        /*
            Get data about networkConfig

            Precondition these values must already have been set
        */
        ipVersion getIpVersion() const;
        std::string getIpAddress() const;
        portType getPort() const;

        /*
            Set data about networkConfig

            Precondition you must set the ip version first
        */
        networkingConfig& setIpVersion(ipVersion version);
        networkingConfig& setIpAddress(const std::string& ipAddress);
        networkingConfig& setPort(portType port);

        /*
            Return the sockaddr representation for lowlevel networking functions
        */
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

    enum class connectionStatus : int {failure = -1, disconnected, success};
    
    /*
        A simple client to connect, send and receive data from a server
    */
    class client
    {
        public:
        /*
            Set client attributes
        */
        client(ipVersion version, protocal setProtocal);

        client(const client& copyFrom) = delete;
        client& operator=(const client& copyFrom) = delete;

        client(client&& moveFrom) = default;
        client& operator=(client&& moveFrom) = default;
        
        /*
            Close socket handle
        */
        ~client();

        /*
            Get information about the client
        */
        ipVersion getIpVersion() const;
        protocal getProtocal() const;

        /*
            Connect to a server 

            Note dns does not work so ip addresses only
        */
        connectionStatus connect(ipVersion serverIpVersion, const std::string& serverIpAddress, portType serverPort);

        /*
            Send and get data from server

            Note receive overwrites string
        */
        connectionStatus send(const std::string& data);
        connectionStatus receive(std::string& data);

        /*
            Close the socket
        */
        void disconnect();

        private:
        ipVersion clientIpVersion;
        protocal clientProtocal;
        socketType socketHandle;
    };

    /*
        Use client like standard library streams
    */
    client& operator<<(client& into, const std::string& data);
    client& operator>>(client& out, std::string& data);

    class server
    {
    };
} //networking
