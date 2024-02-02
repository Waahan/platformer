#include "networking.h"

#include <utility>

#if defined(__WINDOWS__)
    //Because winsock only has WSAPoll
    inline int poll(struct pollfd* pollfdArrray, unsigned long elementsOfPollfdArray, int timeout) { return WSAPoll(pollfdArray, elementsOfPollfdArray, timeout); }
#else
    extern "C"
    {
        #include <poll.h> //For poll
    }
#endif

namespace
{
    #if defined(__WINDOWS__)
        using transmissionType = char*;
        using transmissionTypeConst = const char*; //Because of compiler bug below 
        using transmissionLengthType = int; 
        using transmissionReturnType = int;

        std::string getSocketError()
        {
        /*
            Return a error message string for winsock errors
        */
            int errorCode = WSAGetLastError();

            char* messageBuffer;
            runtime_assert((FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 0, nullptr) != 0), "Failed to make error string, good luck.");

            std::string errorMessage{static_cast<const char*>(messageBuffer)};

            LocalFree(messageBuffer);

            return errorMessage;
        }

        Estd::initGuard<std::function<void(void)>> startNetworking(const std::string& windowsVersion)
        {
        /*
            Init winsock and use desired windowsVersion
            
            Precondition between 1.0 and 2.2 or latest version
            Precondition 3 characters
        */
            debug_assert(( (windowsVersion.size() / sizeof(std::string::value_type)) == 3 ), "windowsVersion string must be 3 characters long. Ex 2.2");

            WSADATA wsaData{};

            runtime_assert((WSAStartup(MAKEWORD(windowsVersion[0], windowsVersion[2]), &wsaData) == 0), "WSAStartup failed. Error: " + getSocketError());

            return Estd::initGuard<std::function<void(void)>>{endNetworking};
        }
    #else
        using transmissionType = void*;
        using transmissionTypeConst = const void*; //Because of a compile error with reinterpret_cast
        using transmissionLengthType = size_t;
        using transmissionReturnType = ssize_t;
    #endif

    enum class protocal : int 
    {
        tcp=SOCK_STREAM,
        udp=SOCK_DGRAM
    };

    Estd::custom_unique_ptr<addrinfo, freeaddrinfo> getLocalAddressInfo(protocal setProtocal, const std::string& port = "4444")
    {
    /*
        Create an addrinfo struct for the current host
    */
        addrinfo getAddressInfoHints{};
        getAddressInfoHints.ai_family = AF_UNSPEC; //Use whatever is the current ipv
        getAddressInfoHints.ai_socktype = static_cast<int>(setProtocal); //Use requested protocal
        getAddressInfoHints.ai_flags = AI_PASSIVE; //Make it prepared for binding

        struct addrinfo* resultOfGetAddressInfo;

        //Use nullptr to use the local host
        runtime_assert((getaddrinfo(nullptr, port.c_str(), &getAddressInfoHints, &resultOfGetAddressInfo) == 0), "getaddrinfo failed. Error: " + networking::getSocketError());

        return Estd::custom_unique_ptr<addrinfo, freeaddrinfo>{resultOfGetAddressInfo};
    }

    networking::socketType makeSocketFromAddressInfo(const addrinfo* const addressInfo)
    {
    /*
        Create a socket from addressinfo
        
        Precondition addressInfo is not a nullptr
    */
        debug_assert((addressInfo), "addressInfo can not be a nullptr");

        networking::socketType newSocket{socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol)};

        runtime_assert((newSocket > 0), "Failed to create socket. Error: " + networking::getSocketError());

        return newSocket; 
    }

    void bindSocket(networking::socketType bindingSocket, const networking::socketAddress& bindingGuide)
    {
    /*
        Bind a socket 

        Precondtion port and socket address information must already be set
    */
        //Allow socket to easily be bound again
        {
            int yes = 1;

            debug_assert((setsockopt(bindingSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 0), "Failed to set socket option. Error: " + networking::getSocketError());
        }

        runtime_assert((bind(bindingSocket, bindingGuide.sockaddr(), bindingGuide.size()) == 0), "Failed to bind socket. Error: " + networking::getSocketError());
    }

    networking::socketAddress resolveHost(const std::string& hostname, const std::string& port, protocal agreedProtocal)
    {
    /*
        Converts dns, ips, ports and other information into a socket address useable for other networking functions

        Precondition both the hostname and port must be vailed ips, domains, port numbers or services 
        Precondition you and the host use the same protocal
    */
        struct addrinfo hints{};
        hints.ai_family = AF_UNSPEC; //ipv4 or ipv6
        hints.ai_socktype = static_cast<int>(agreedProtocal);
        hints.ai_flags = AI_PASSIVE; //Make it able to bind and connect

        struct addrinfo* resultAddress;

        debug_assert((getaddrinfo(hostname.c_str(), port.c_str(), &hints, &resultAddress) == 0), "getaddrinfo failed. Error: " + networking::getSocketError());

        Estd::custom_unique_ptr<struct addrinfo, freeaddrinfo> addressInfo{resultAddress};

        return networking::socketAddress{addressInfo.get()};
    }

    bool canRead(networking::socketType socketHandle)
    {
    /*
        Check will poll if you can read from a socket without blocking
    */
        struct pollfd polling{};
        polling.fd = socketHandle; //Set socket to poll
        polling.events = POLLRDNORM; //Can data be read without blocking

        debug_assert((poll(&polling, 1, 30000) > 0), "Poll failed. Error: " + networking::getSocketError());

        return (polling.revents == POLLRDNORM);
    }

    networking::connectionStatus sendString(networking::socketType socketHandle, const std::string& data)
    {
    /* 
        Send data throught socket
    */
        int dataSize = data.size() * sizeof(std::string::value_type);

        //reinterpret_cast must be used because of void pointers 
        transmissionReturnType sendResult = ::send(socketHandle, reinterpret_cast<transmissionTypeConst>(data.c_str()), static_cast<transmissionLengthType>(dataSize), 0);

        return (sendResult > 0) ? networking::connectionStatus::connected : static_cast<networking::connectionStatus>(sendResult);
    }
    
    networking::connectionStatus receiveString(networking::socketType socketHandle, std::string& buffer, int max)
    {
    /*
        Receive data from socketHandle
    */
        //Allocate memory
        std::unique_ptr<transmissionType> outputBuffer{new transmissionType[max]};
        buffer.reserve(buffer.size() + max);

        //Get data
        transmissionReturnType receiveResult = ::recv(socketHandle, outputBuffer.get(), static_cast<transmissionLengthType>(max), 0);

        //Store it
        if(receiveResult > 0)
            //reinterpret_cast must be used because we are using void pointers
            buffer.append(reinterpret_cast<const char*>(outputBuffer.get()), receiveResult);

        //Return result
        return (receiveResult > 0) ? networking::connectionStatus::connected : static_cast<networking::connectionStatus>(receiveResult);
    }
} //namespace

namespace networking 
{
    socketAddress::socketAddress(struct addrinfo* addressInfo)
        : lowLevelSocketAddress{*addressInfo->ai_addr}, lowLevelAddressLength{addressInfo->ai_addrlen}
    {
    /*
        Create a socket address from addrinfo. Create a deep copy because addressInfo might be destroyed.

        Precondtion addressInfo is not NULL
        Preconditon addressInfo->ai_addr has already been set
    */
    } 

    socketAddress::socketAddress(struct sockaddr&& createSocketAddress)
        : lowLevelSocketAddress{std::move(createSocketAddress)}, lowLevelAddressLength{sizeof(createSocketAddress)}
    {
    /*
        Create a socket address from sockaddr 

        Preconditon must be the size of sockaddr
    */
    }

    tcpClient::tcpClient()
        : socketHandle{}, ownAddress{}
    {
        Estd::custom_unique_ptr<addrinfo, freeaddrinfo> ourAddressInfo = getLocalAddressInfo(protocal::tcp);

        socketHandle = makeSocketFromAddressInfo(ourAddressInfo.get());
        ownAddress = socketAddress{ourAddressInfo.get()};
    }

    bool tcpClient::pendingData() const 
    {
    /*
        Check if the client socket has any data to read
    */
        return canRead(static_cast<socketType>(socketHandle)); 
    }

    connectionStatus tcpClient::connect(const std::string& hostname, const std::string& port)
    {
    /*
        Connect to a tcp server
    */
        socketAddress server = resolveHost(hostname, port, protocal::tcp);

        return !(::connect(static_cast<socketType>(socketHandle), static_cast<const sockaddr*>(server.sockaddr()), server.size())) ? connectionStatus::connected : connectionStatus::error;
    }

    connectionStatus tcpClient::send(const std::string& data)
    {
    /*
        Send string data with the clients socket
    */
        return sendString(static_cast<socketType>(socketHandle), data);
    }

    connectionStatus tcpClient::receive(std::string& buffer, int max)
    {
    /*
        Receive string data and write it the the buffer
    */
        return receiveString(static_cast<socketType>(socketHandle), buffer, max);
    }

    bool tcpServerClient::pendingData() const
    {
    /*
        Find out if server client has sent information
    */
        return canRead(static_cast<socketType>(socketHandle));
    }

    connectionStatus tcpServerClient::send(const std::string& data)
    {
    /*
        Send data to serverClient
    */
        return sendString(static_cast<socketType>(socketHandle), data);
    }

    connectionStatus tcpServerClient::receive(std::string& buffer, int max)
    {
    /*
        Get sent data from server client
    */
        return receiveString(static_cast<socketType>(socketHandle), buffer, max);
    }

    tcpServer::tcpServer(const std::string& port)
        : socketHandle{}, ownAddress{}
    {
    /*
        Create the base tcp socket and sockaddr
    */
        Estd::custom_unique_ptr<addrinfo, freeaddrinfo> ourAddressInfo = getLocalAddressInfo(protocal::tcp, port);

        socketHandle = makeSocketFromAddressInfo(ourAddressInfo.get());
        ownAddress = socketAddress{ourAddressInfo.get()};
    }

    void tcpServer::bind()
    {
    /*
        Bind tcpServers socket and make it a listening socket
    */
        bindSocket(static_cast<socketType>(socketHandle), ownAddress);

        runtime_assert((listen(static_cast<socketType>(socketHandle), 100) == 0), "Failed to make socket listen. Error: " + getSocketError());
    }

    bool tcpServer::pendingData() const
    {
    /*
        Check if there are pending clients on listening socket
    */
        return canRead(static_cast<socketType>(socketHandle));
    }

    std::unique_ptr<baseClient> tcpServer::acceptClient() const
    {
    /*
        Use accept to accept pending clients
    */
        sockaddr clientSocketAddress{};
        socketAddressLength clientSocketAddressLength{sizeof(sockaddr)};

        socketType clientSocketHandle = accept(static_cast<socketType>(socketHandle), &clientSocketAddress, &clientSocketAddressLength);

        runtime_assert((clientSocketHandle != -1), "Failed to accept client. Error " + getSocketError());

        return std::unique_ptr<baseClient>{new tcpServerClient{clientSocketHandle, socketAddress{std::move(clientSocketAddress)}}};
    }
} //networking
