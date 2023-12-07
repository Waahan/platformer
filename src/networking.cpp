#include "networking.h"

#ifdef __WINDOWS__
#else
    #include <errno.h>
#endif

#include "errors.h"

namespace
{
    int getNetworkingErrorCode()
    {
    /*
        Get error code for unix or windows
    */
    #ifdef __WINDOWS__
        return WSAGetLastError();
    #else
        return errno;
    #endif
    }

    socketType createSocket(networking::ipVersion setIpVersion, networking::protocal setProtocal)
    {
    /*
        Create a socket

        Postcondition does not return invalid socket
    */
        socketType newSocket = socket((int)setIpVersion, (int)setProtocal, 0);
    
    #ifdef __WINDOWS__ 
        constexpr const int socketError = INVALID_SOCKET;
    #else
        constexpr const int socketError = -1;
    #endif

        runtime_assert((newSocket != socketError), "Error failed to create socket. Error code: " + getNetworkingErrorCode());

        return newSocket;
    }

    void closeSocket(socketType socketHandle)
    {
    /*
        Close a socket

        Postcondition close socket function returns 0 
    */
    #ifdef __WINDOWS__
        int closeSocketResult = closesocket(socketHandle);
    #else
        int closeSocketResult = close(socketHandle);
    #endif
        runtime_assert((closeSocketResult == 0), "Failed to close socket. Error code: " + getNetworkingErrorCode());
    }

    /*
        Because of how bits are stored you have to convert to and from network byte order
        with these functions
    */
    inline unsigned short hostToNetworkShort(unsigned short hostShort)
    {
        return htons(hostShort);
    }

    inline unsigned int hostToNetworkLong(unsigned int hostLong)
    {
        return htonl(hostLong);
    }

    inline unsigned short networkToHostShort(unsigned short networkShort)
    {
        return ntohs(networkShort);
    }

    inline unsigned int networkToHostLong(unsigned int networkLong)
    {
        return ntohl(networkLong);
    }

    void ipAddressToBinary(networking::ipVersion ipAddressIpVersion, const std::string& ipAddress, void* destination)
    {
    /*
        Write the human reable string ipAddress to a binary ip address at destinaion

        Precondition destination is not null
        Postcondition inet_pton returns 1
    */
        debug_assert((destination), "Error can not use nullptr pointer to write binary ip to");

        runtime_assert((inet_pton((int)ipAddressIpVersion, ipAddress.c_str(), destination) == 1), "Error unable to convert ip string to binary. Error code: " + getNetworkingErrorCode());
    }

    std::string binaryToIpAddress(networking::ipVersion ipAddressIpVersion, const void* binaryIp)
    {
    /*
        Make binary ip into a human readable string

        Precondition binaryIp is not null
    */
        debug_assert((binaryIp), "Error can not convert null to ip address string");

        constexpr const int maxIpAddressSize = 46;
        constexpr const size_t ipAddressBufferSize = sizeof(std::string::value_type) * maxIpAddressSize;

        char ipAddressBuffer[maxIpAddressSize];
        char* ipAddressBufferPointer = &ipAddressBuffer[0];

        const char* ipAddressString = inet_ntop((int)ipAddressIpVersion, binaryIp, ipAddressBufferPointer, ipAddressBufferSize);

        runtime_assert((ipAddressString), "Error in converting binary ip to string. Error code: " + getNetworkingErrorCode());

        return std::string(ipAddressString);
    }
} //namespace

namespace networking
{
    networkingConfig::networkingConfig(struct sockaddr&& convert)
    {
    /*
        Convert sockaddr to a networkConfig

        Precondition sockaddr is ipv4 or ipv6
    */
        switch(convert.sa_family)
        {
            case AF_INET:
                currentIpVersion = ipVersion::ipv4;
                socketAddressIpv4 = std::move((struct sockaddr_in&&)convert);
                break;

            case AF_INET6:
                currentIpVersion = ipVersion::ipv6;
                socketAddressIpv6 = std::move((struct sockaddr_in6&&)convert);
                break;

            default:
                debug_error("Unsupported sockaddr given");
                break;
        }
    }

    ipVersion networkingConfig::getIpVersion() const
    {
    /*
        Return the ipVersion currently being used 

        Precondition the ipVersion has been set
    */
        return currentIpVersion;
    }

    std::string networkingConfig::getIpAddress() const 
    {
    /*
        Return a human readable ip address from the sockaddr 

        Precondition the ip address has been set
    */
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                return binaryToIpAddress(currentIpVersion, (void*)&socketAddressIpv4.sin_addr);

            case ipVersion::ipv6:
                return binaryToIpAddress(currentIpVersion, (void*)&socketAddressIpv6.sin6_addr);
        }

        return "Failed to get ip address";
    }

    portType networkingConfig::getPort() const
    {
    /*
        Return a port number from socketaddr
    */
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                return networkToHostShort(socketAddressIpv4.sin_port);

            case ipVersion::ipv6:
                return networkToHostShort(socketAddressIpv6.sin6_port);
        }

        return 0;
    }

    networkingConfig& networkingConfig::setIpVersion(ipVersion version)
    {
    /*
        Set the current ip address version
    */
        currentIpVersion = version;

        return *this;
    }

    networkingConfig& networkingConfig::setIpAddress(const std::string& ipAddress)
    {
    /*
        Set the ip address

        Precondition ip address is valid
    */
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                ipAddressToBinary(currentIpVersion, ipAddress.c_str(), (void*)&socketAddressIpv4.sin_addr);
                break;

            case ipVersion::ipv6:
                ipAddressToBinary(currentIpVersion, ipAddress.c_str(), (void*)&socketAddressIpv6.sin6_addr);
                break;
        }

        return *this; 
    }

    networkingConfig& networkingConfig::setPort(portType port)
    {
    /*
        Set the port number

        Precondition port is above 0 and below 65535
    */
        debug_assert((port >= 0 && port <= 65535), "Invalid port number");

        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                socketAddressIpv4.sin_port = hostToNetworkShort(port);
                break;

            case ipVersion::ipv6:
                socketAddressIpv6.sin6_port = hostToNetworkShort(port);
                break;
        }

        return *this;
    }

    const struct sockaddr* networkingConfig::sockaddrRep() const
    {
    /*
        Return the sockaddr representation of the class 
    */
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                return (struct sockaddr*)&socketAddressIpv4; 

            case ipVersion::ipv6:
                return (struct sockaddr*)&socketAddressIpv6; 
        }
 
        return nullptr;
    }
    
    struct sockaddr* networkingConfig::sockaddrRep() 
    {
    /*
        Return the sockaddr representation of the class 
    */
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                return (struct sockaddr*)&socketAddressIpv4;

            case ipVersion::ipv6:
                return (struct sockaddr*)&socketAddressIpv6;
        }

        return nullptr;
    }

    size_t networkingConfig::size()
    {
        switch(currentIpVersion)
        {
            case ipVersion::ipv4:
                return sizeof(socketAddressIpv4);

            case ipVersion::ipv6:
                return sizeof(socketAddressIpv6);
        }

        return -1;
    }
} //networking
