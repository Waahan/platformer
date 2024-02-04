#pragma once

#include <string>
#include <memory>
#include <functional>

//For operating system specific headers 
#if defined(__WINDOWS__)
    //WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    //Windows headers
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
#else
    //Unix headers 
    #include <cerrno> // Error handling
    #include <cstring> // Also for error handling (std::strerror)

    extern "C"
    {
        #include <sys/types.h> // For addrinfo
        #include <sys/socket.h> // For socket and addrinfo
        #include <netdb.h> // For addrinfo
        #include <unistd.h> //For close
    }
#endif
 
#include "errors.h"
#include "Estd.h"

namespace networking
{
    #if defined(__WINDOWS__)
        //Winsock specific stuff
        using socketType = SOCKET;
        using socketAddressLength = int;

        std::string getSocketError(); //Exposes socket errors for client and server send and recv errors

        inline void endNetworking() 
        { 
            [[maybe_unused]] const int cleanupResult = WSACleanup();

            debug_assert((cleanupResult == 0), "WSACleanup failed. Error: " + getSocketError()); 
        }

        Estd::initGuard<std::function<void(void)>> startNetworking(const std::string& windowsVersion);

        inline void closeSocket(socketType destroySocket) 
        { 
            [[maybe_unused]] const int closeSocketResult = closesocket(destroySocket);

            debug_assert((closeSocketResult == 0), "closesocket failed. Error: " + getSocketError()); 
        }
    #else 
        //Unix specific stuff
        using socketType = int;
        using socketAddressLength = socklen_t;

        inline std::string getSocketError() { return std::strerror(errno); } //Exposes socket errors for client and server send and recv errors

        //Because winsock needs WSAStartup and WSACleanup
        inline void endNetworking() { /* No init no cleanup */ }
        inline Estd::initGuard<std::function<void(void)>> startNetworking(const std::string&) { return Estd::initGuard<std::function<void(void)>>(endNetworking); /* No init function for unix socket api */ }

        inline void closeSocket(socketType destroySocket) 
        { 
            [[maybe_unused]] const int closeResult = close(destroySocket);

            debug_assert((closeResult == 0), "Failed to close socket. Error: " + getSocketError()); 
        } //Because of the return type of close and custom_unique_ptrs templates
    #endif

    class socketAddress
    {
        public:
        socketAddress(struct addrinfo* addressInfo); //Note ownership of addrinfo is not passed to socketAddress, must be filled in already
        socketAddress(struct sockaddr&& createSocketAddress);
        socketAddress() = default;

        socketAddress(const socketAddress& copyFrom) = default;
        socketAddress& operator=(const socketAddress& copyFrom) = default;

        socketAddress(socketAddress&& moveFrom) = default;
        socketAddress& operator=(socketAddress&& moveFrom) = default;

        ~socketAddress() = default;

        inline const struct sockaddr* sockaddr() const { return &lowLevelSocketAddress; }
        inline struct sockaddr* sockaddr() { return &lowLevelSocketAddress; }

        inline int size() const { return lowLevelAddressLength; }

        private:
        struct sockaddr lowLevelSocketAddress;
        socketAddressLength lowLevelAddressLength;
    };

    enum connectionStatus
    {
        error = -1,
        connected,
        disconnected,
    };

    class baseSocketClass
    {
        public:
        baseSocketClass() = default;

        baseSocketClass(const baseSocketClass& copyFrom) = delete;
        baseSocketClass& operator=(const baseSocketClass& copyFrom) = delete;

        baseSocketClass(baseSocketClass&& moveFrom) = delete;
        baseSocketClass& operator=(baseSocketClass&& moveFrom) = delete;

        virtual ~baseSocketClass() {}

        virtual bool pendingData() const =0;
    };

    class baseClient : public baseSocketClass
    {
        public:
        baseClient() = default;

        baseClient(const baseClient& copyFrom) = delete;
        baseClient& operator=(const baseClient& copyFrom) = delete;

        baseClient(baseClient&& moveFrom) = delete;
        baseClient& operator=(baseClient&& moveFrom) = delete;

        ~baseClient() override {}

        bool pendingData() const override =0;

        virtual connectionStatus send(const std::string& data) =0;
        virtual connectionStatus receive(std::string& buffer, int max) =0;
    };

    class client : public baseClient 
    {    
        public:
        client() = default;

        client(const client& copyFrom) = delete;
        client& operator=(const client& copyFrom) = delete;

        client(client&& moveFrom) = delete;
        client& operator=(client&& moveFrom) = delete;

        ~client() override {}

        bool pendingData() const override =0;

        virtual connectionStatus connect(const std::string& hostname, const std::string& port) =0;

        connectionStatus send(const std::string& data) override =0;
        connectionStatus receive(std::string& buffer, int max) override =0;
    };

    class server : public baseSocketClass
    {
        public:
        server() = default;

        server(const server& copyFrom) = delete;
        server& operator=(const server& copFrom) = delete;
        
        server(server&& moveFrom) = delete;
        server& operator=(server&& moveFrom) = delete;

        ~server() override {}

        virtual void bind() =0; //Must do this before other operations

        bool pendingData() const override =0;

        virtual std::unique_ptr<baseClient> acceptClient() const =0;
    };

    class tcpClient : public client
    {
        public:
        tcpClient();

        tcpClient(const tcpClient& copyFrom) = delete;
        tcpClient& operator=(const tcpClient& copyFrom) = delete;

        tcpClient(tcpClient&& moveFrom) = default;
        tcpClient& operator=(tcpClient&& moveFrom) = default;

        ~tcpClient() = default;

        bool pendingData() const override;

        connectionStatus connect(const std::string& hostname, const std::string& port) override;

        connectionStatus send(const std::string& data) override;
        connectionStatus receive(std::string& buffer, int max) override;

        private:
        Estd::fileDescriptorHandle<socketType, closeSocket> socketHandle;
        socketAddress ownAddress;
    };
    
    class tcpServerClient : public baseClient
    {
        friend class tcpServer;
        public:
        tcpServerClient() = delete;

        tcpServerClient(const tcpServerClient& copyFrom) = delete;
        tcpServerClient& operator=(const tcpServerClient& copyFrom) = delete;

        tcpServerClient(tcpServerClient&& moveFrom) = default;
        tcpServerClient& operator=(tcpServerClient&& moveFrom) = default;

        ~tcpServerClient() = default;

        bool pendingData() const override;

        connectionStatus send(const std::string& data) override;
        connectionStatus receive(std::string& buffer, int max) override;

        private:
        tcpServerClient(socketType setSocketHandle, socketAddress&& setAddress) : socketHandle{setSocketHandle}, ownAddress{setAddress} {}

        Estd::fileDescriptorHandle<socketType, closeSocket> socketHandle;
        socketAddress ownAddress; 
    };

    class tcpServer : public server
    {
        public:
        tcpServer(const std::string& port);

        tcpServer(const tcpServer& copyFrom) = delete;
        tcpServer& operator=(const tcpServer& copyFrom) = delete;

        tcpServer(tcpServer&& moveFrom) = default;
        tcpServer& operator=(tcpServer&& moveFrom) = default;

        ~tcpServer() override = default;

        void bind() override;

        bool pendingData() const override;

        std::unique_ptr<baseClient> acceptClient() const override;

        private:
        Estd::fileDescriptorHandle<socketType, closeSocket> socketHandle;
        socketAddress ownAddress;
    };
} //networking
