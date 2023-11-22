#pragma once

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
#else
    #include <sys/socket.h>
    #include <sys/types.h> 
    #include <arpa/inet.h>
    #include <unistd.h>

    //Unix socket representations
    typedef int socketType;
    typedef socklen_t socketLengthType;
#endif
}

namespace networking
{
    enum class protocal : int {tcp = SOCK_STREAM, udp = SOCK_DGRAM};
    enum class ipType : int {ipv4 = AF_INET, ipv6 = AF_INET6};

    class server
    {
    };

    class client
    {
    };
} //networking
