#include <stdio.h>

#include "socket.h"

Socket::Socket()
{

}

Socket::~Socket()
{

}

void Socket::Close()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close(this->handle);
#elif PLATFORM == PLATFORM_WINDOWS
    closesocket(this->handle);
#endif
}

bool Socket::IsOpen() const
{
    return this->handle > 0;
}

bool Socket::Open(unsigned short port)
{
    this->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->handle <= 0)
    {
        printf("failed to create socket\n");
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)port);
    if (bind(this->handle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
    {
        printf("failed to bind socket\n");
        return false;
    }
    printf("Listening on port %d\n", port);

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int non_blocking = 1;
    if (fcntl(this->handle, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to set non-blocking\n");
        return false;
    }
#elif PLATFORM == PLATFORM_WINDOWS
    DWORD non_blocking = 1;
    if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0)
    {
        printf("failed to set non-blocking\n");
        return false;
    }
#endif

    return true;
}

int Socket::Receive(Address &sender, void *data, int size)
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif

    sockaddr_in from;
    socklen_t from_length = sizeof(from);

    int bytes = recvfrom(this->handle, (char*)data, size, 0, (sockaddr*)&from,
        &from_length);

    wchar_t from_address[16];
    InetNtop(AF_INET, &from.sin_addr, from_address, 16);
    unsigned int from_port = ntohs(from.sin_port);
    sender = Address(from_address, from_port);

    return bytes;
}

bool Socket::Send(Address &destination, const void *data, int size)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    wchar_t temp[16];
    destination.GetAddress(temp);
    int error = InetPton(AF_INET, temp, &addr.sin_addr);
    addr.sin_port = htons(destination.GetPort());

    int sent_bytes = sendto(this->handle, (const char*)data, size, 0,
        (sockaddr*)&addr, sizeof(sockaddr_in));
    if (sent_bytes != size)
    {
        printf("failed to send packet\n");
        return false;
    }

    return true;
}
