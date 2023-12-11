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

        socketLengthType size() const;

        private:
        ipVersion currentIpVersion;

        union
        {
            sockaddr_in socketAddressIpv4; 
            sockaddr_in6 socketAddressIpv6;
        };
    };

    enum class connectionStatus : int { 
    #ifdef __WINDOWS__
        error = SOCKET_ERROR,
    #else
        error = -1, 
    #endif
        disconnected = 0, 
        connected = 1 
    };

    class networkingExchange
    {
        public:
        networkingExchange() = default;

        networkingExchange(const networkingExchange& copyFrom) = delete;
        networkingExchange& operator=(const networkingExchange& copyFrom) = delete;

        networkingExchange(networkingExchange&& moveFrom) = delete;
        networkingExchange& operator=(networkingExchange&& moveFrom) = delete;

        virtual ~networkingExchange() = default;
        
        virtual connectionStatus send(const std::string& buffer) =0;
        virtual connectionStatus receive(std::string& buffer, const int maxReceive) =0;

        virtual connectionStatus send(const std::string& buffer, const networkingConfig& receipient) =0;
        virtual connectionStatus receive(std::string& buffer, const int maxReceive, networkingConfig& sender) =0;

        virtual void close() =0;
    };

    inline networkingExchange& operator<<(networkingExchange& os, const std::string& buffer)
    {
        os.send(buffer);

        return os;
    }

    inline networkingExchange& operator>>(networkingExchange& is, std::string& buffer)
    {
        constexpr const int maxReceive = 100;

        is.receive(buffer, maxReceive);

        return is;
    }

    class client : public networkingExchange
    {
        public:
        client(ipVersion setIpVersion, protocal setProtocal);

        client(const client& copyFrom) = delete;
        client& operator=(const client& copyFrom) = delete;

        client(client&& moveFrom) = delete;
        client& operator=(client&& moveFrom) = delete;

        ~client() override;

        connectionStatus connect(const networkingConfig& serverConfig);

        connectionStatus send(const std::string& buffer) override;
        connectionStatus receive(std::string& buffer, const int maxReceive) override;

        connectionStatus send(const std::string& buffer, const networkingConfig& receipient) override;
        connectionStatus receive(std::string& buffer, const int maxReceive, networkingConfig& sender) override;

        void close() override;

        private:
        ipVersion version;
        socketType socketHandle;
    };
} //Networking
