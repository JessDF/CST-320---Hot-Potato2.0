#ifndef SOCKET_H
#define SOCKET_H

// platform detection
#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <Ws2tcpip.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #pragma comment(lib, "ws2_32.lib")
#endif

#include "address.h"

class Socket {
public:
    Socket();
    ~Socket();
    void Close();
    bool IsOpen() const;
    bool Open(unsigned short port);
    int Receive(Address &sender, void *data, int size);
    bool Send(Address &destination, const void *data, int size);
private:
    int handle;
};

#endif
