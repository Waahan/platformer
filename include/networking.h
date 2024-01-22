#pragma once

#include <string>
#include <tuple>
#include <memory>
#include <functional>

//For operating system specific headers 
#if defined(__WINDOWS__)
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
    #else 
        //Unix specific stuff
        typedef int socketType;

        //Because winsock needs WSAStartup and WSACleanup
        inline void endNetworking() { /* No init no cleanup */ }
        inline Estd::initGuard<std::function<void(void)>> startNetworking(const std::string) { return Estd::initGuard<std::function<void(void)>>(endNetworking); /* No init function for unix socket api */ }

        inline std::string getSocketError() { return std::strerror(errno); } //Exposes socket errors for client and server send and recv errors

        inline void closeSocket(socketType destroySocket) { debug_assert(close(destroySocket) == 0, "Failed to close socket. Error: " + getSocketError()); } //Because of the return type of close and custom_unique_ptrs templates
    #endif

    class socketAddress
    {
        public:
        socketAddress() = delete;

        socketAddress(const socketAddress& copyFrom) = default;
        socketAddress& operator=(const socketAddress& copyFrom) = default;

        socketAddress(socketAddress&& moveFrom) = default;
        socketAddress& operator=(socketAddress&& moveFrom) = default;

        ~socketAddress() = default;

        private:
        //socketAddress(addrinfo* addressInfo); //Note ownership of addrinfo is passed to socketAddress, must be filled in already
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

        bool pendingData() const override =0;

        virtual std::unique_ptr<baseClient> acceptClient() const =0;
    };
} //networking
