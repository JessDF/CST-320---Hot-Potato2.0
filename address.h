#ifndef ADDRESS_H
#define ADDRESS_H

class Address
{
public:
    Address();
    Address(wchar_t address[], unsigned short port);
    void GetIp(wchar_t temp[]) const;
    unsigned short GetPort() const;
private:
    void init(wchar_t address[], unsigned short port);
    wchar_t address[16];
    unsigned short port;
};

#endif
