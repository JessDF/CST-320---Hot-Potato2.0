#include "address.h"
#include <wchar.h>

Address::Address()
{
    init(L"0.0.0.0", 0);
}

Address::Address(wchar_t address[], unsigned short port)
{
    init(address, port);
}

void Address::init(wchar_t address[], unsigned short port)
{
    wcsncpy_s(this->address, 16, address, 16);
    this->port = port;
}

void Address::GetIp(wchar_t temp[]) const
{
    wcsncpy_s(temp, 16, this->address, 16);
}

unsigned short Address::GetPort() const
{
    return this->port;
}
