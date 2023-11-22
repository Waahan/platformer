#include "networking.h"

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

    inline unsigned int networkToHostInt(unsigned int networkLong)
    {
        return ntohl(networkLong);
    }

    void cleanUpSocket(socketType destroySocket)
    {
    #ifdef __WINDOWS__
        closesocket(destroySocket);
    #else
        close(destroySocket);
    #endif
    }
} //namespace

namespace networking
{
} //networking
