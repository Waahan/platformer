#include "networking.h"

#include <memory>

#include "errors.h"

namespace
{
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

    inline int getNetworkingError()
    {
    #ifdef __WINDOWS__
        return WSAGetLastError();
    #else
        return errno;
    #endif
    }

    /*
        Convert ip addresses to and from binary
    */
    inline void ipAddressTextToBinary(networking::ipVersion version, const char* ipAddressString, void* destination)
    {
    /*
        Write ip address into destination 

        Precondition destination is not NULL
        Precondition ipAddressString is not NULL

        Postcondition inet_pton returns 1
    */
        debug_assert(ipAddressString, "Must have a valid ip address string");
        debug_assert(destination, "Must have a valid place to write binary ip");

        int doesNotHaveError = inet_pton((int)version, ipAddressString, destination);

        runtime_assert((doesNotHaveError == 1), "Failed to convert ip address Error code: " + doesNotHaveError);
    }

    inline std::string ipAddressFromBinaryToText(networking::ipVersion version, const void* binaryAddress)
    {
    /*
        Convert binary ip addres to a human readable ip address
        
        Precondition valid binaryAddress pointer
    */
        debug_assert(binaryAddress, "Invalid binary address");

        constexpr const int maxIpAddressSize = 45;

        //Add one for the NULL terminator
        char buffer[maxIpAddressSize + 1];

        const char* ipAddressString = inet_ntop((int)version, binaryAddress, buffer, maxIpAddressSize + 1);

        runtime_assert(ipAddressString, "Error in converting binary ip: " + getNetworkingError());

        return ipAddressString;
    }

    inline void cleanUpSocket(socketType destroySocket)
    {
    #ifdef __WINDOWS__
        closesocket(destroySocket);
    #else
        close(destroySocket);
    #endif
    }

    #ifdef __WINDOWS__
    typedef int sizeType;
    #else 
    typedef size_t sizeType;
    #endif

    inline sizeType sendData(socketType to, const char* data, sizeType sizeofData)
    {
    #ifdef __WINDOWS__
        return send(to, data, sizeOfData, 0);
    #else
        return send(to, (void*)data, sizeofData, 0);
    #endif
    }

    inline sizeType receiveData(socketType from, char* buffer, sizeType sizeofBuffer)
    {
    #ifdef __WINDOWS__
        return recv(from, buffer, sizeOfBuffer, 0);
    #else
        return recv(from, (void*)buffer, sizeofBuffer, 0);
    #endif
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
                currentVersion = ipVersion::ipv4;
                socketAddress.ipv4SocketAddress = std::move((struct sockaddr_in&&)convert);
                break;

            case AF_INET6:
                currentVersion = ipVersion::ipv6;
                socketAddress.ipv6SocketAddress = std::move((struct sockaddr_in6&&)convert);
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
        return currentVersion;
    }

    std::string networkingConfig::getIpAddress() const 
    {
    /*
        Return a human readable ip address from the sockaddr 

        Precondition the ip address has been set
    */
        switch(currentVersion)
        {
            case ipVersion::ipv4:
                return ipAddressFromBinaryToText(currentVersion, (void*)&socketAddress.ipv4SocketAddress.sin_addr);

            case ipVersion::ipv6:
                return ipAddressFromBinaryToText(currentVersion, (void*)&socketAddress.ipv6SocketAddress.sin6_addr);
        }

        return "Failed to get ip address";
    }

    portType networkingConfig::getPort() const
    {
    /*
        Return a port number from socketaddr
    */
        switch(currentVersion)
        {
            case ipVersion::ipv4:
                return networkToHostShort(socketAddress.ipv4SocketAddress.sin_port);

            case ipVersion::ipv6:
                return networkToHostShort(socketAddress.ipv6SocketAddress.sin6_port);
        }

        return 0;
    }

    networkingConfig& networkingConfig::setIpVersion(ipVersion version)
    {
    /*
        Set the current ip address version
    */
        currentVersion = version;

        return *this;
    }

    networkingConfig& networkingConfig::setIpAddress(const std::string& ipAddress)
    {
    /*
        Set the ip address

        Precondition ip address is valid
    */
        switch(currentVersion)
        {
            case ipVersion::ipv4:
                ipAddressTextToBinary(currentVersion, ipAddress.c_str(), (void*)&socketAddress.ipv4SocketAddress.sin_addr);
                break;

            case ipVersion::ipv6:
                ipAddressTextToBinary(currentVersion, ipAddress.c_str(), (void*)&socketAddress.ipv6SocketAddress.sin6_addr);
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

        switch(currentVersion)
        {
            case ipVersion::ipv4:
                socketAddress.ipv4SocketAddress.sin_port = hostToNetworkShort(port);
                break;

            case ipVersion::ipv6:
                socketAddress.ipv6SocketAddress.sin6_port = hostToNetworkShort(port);
                break;
        }

        return *this;
    }

    const struct sockaddr* networkingConfig::sockaddrRep() const
    {
    /*
        Return the sockaddr representation of the class 
    */
        return (struct sockaddr*)&socketAddress;
    }

    client::client(ipVersion version, protocal setProtocal)
    {
    /*
        Open a socket from data about client

        Postcondition socket does not return -1
    */
        clientIpVersion = version;
        clientProtocal = setProtocal;
        
        socketHandle = socket((int)clientIpVersion, (int)clientProtocal, 0);

        runtime_assert((socketHandle != -1), "Failed to open socket error code: " + getNetworkingError());
    }

    client::~client()
    {
    /*
        Close socket handle
    */
        disconnect();
    }

    connectionStatus client::connect(ipVersion serverVersion, const std::string& serverIpAddress, portType serverPort)
    {
    /*
        Connect to a server
    */
        networkingConfig serverConfig;

        serverConfig.setIpVersion(serverVersion).setIpAddress(serverIpAddress).setPort(serverPort);

        return (connectionStatus)::connect(socketHandle, serverConfig.sockaddrRep(), sizeof(serverConfig.sockaddrRep()));
    }

    connectionStatus client::send(const std::string& data)
    {
    /*
        Send data to the connected to server

        Precondition connected to server
    */
        sizeType dataSize = data.size() * sizeof(std::string::value_type);
        sizeType result = sendData(socketHandle, data.c_str(), dataSize);
        
        return (result > 0) ? connectionStatus::success : (connectionStatus)result;
    }

    connectionStatus client::receive(std::string& data)
    {
    /*
        Receive data from connect server

        Precondition connected to server
    */
        std::unique_ptr<char> buffer{new char[100]};
        sizeType bufferSize = sizeof(char) * 100;

        sizeType result = receiveData(socketHandle, buffer.get(), bufferSize);

        data.assign(buffer.get(), bufferSize);

        return (result > 0) ? connectionStatus::success : (connectionStatus)result;
    }

    void client::disconnect()
    {
    /*
        Disconnect from server by closing the socket handle
    */
        cleanUpSocket(socketHandle);

        socketHandle = -1;
    }

    client& operator<<(client& into, const std::string& data)
    {
    /*
        Send data to server
    */
        into.send(data);

        return into;
    }

    client& operator>>(client& out, std::string& data)
    {
    /*
        Get data from server
    */
        out.receive(data);

        return out;
    }
} //networking