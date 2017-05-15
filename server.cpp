#include <ctime>
#include <iostream>
#include <stdio.h>
#include <unordered_set>
#include <wchar.h>

#include "gamestate.h"
#include "socket.h"
#include "vector.h"

#define IP_LENGTH 16
#define NUM_PLAYERS 6
#define PORT 27015
#define THRESHOLD 2

Address player_addresses[NUM_PLAYERS];
Address ovserver_address;

MasterState* master_state;
PlayerState* player_state;

Socket sock;

std::clock_t start;
std::unordered_set<int> player_slots;
std::unordered_set<wchar_t[]> ip_set;

double duration;

wchar_t ip[IP_LENGTH];
wchar_t ips[NUM_PLAYERS][IP_LENGTH];

void Broadcast() 
{
    for (const auto &index : player_slots) 
    {
        std::cout << index;
        std::copy(ips[index], ips[index] + IP_LENGTH, ip);
        SendPacket(ip, (void*)master_state, MASTER_STATE_SIZE);
    }
}

bool InitializeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#else
    return true;
#endif
}

void CheckPotato() 
{
    float distance;
    for (const auto &index : player_slots) {
        distance = Vec3Length(master_state->player_positions[index] -
          master_state->potato_position);
        if (distance <= THRESHOLD) master_state->potato_holder = index;
    }
}

void ReceivePacket()
{
    Address sender;
    void* packet;
    int bytes_read = sock.Receive(sender, packet, PLAYER_STATE_SIZE);
    if (bytes_read <= 0) return;

    sender.GetIp(ip);
    std::unordered_set<wchar_t[]>::const_iterator got = ip_set.find(ip);

    player_state = (struct PlayerState*)packet;
    int player_index = player_state->player_index;

    if (got == ip_set.end() && ips[player_index] == NULL) 
    {
        std::copy(ip, ip + IP_LENGTH, ips[player_index]);
        player_slots.insert(player_index);
    }
    else 
    {
        XMFLOAT3 player_position = player_state->player_position;
        master_state->player_positions[player_index].x = player_position.x;
        master_state->player_positions[player_index].y = player_position.y;
        master_state->player_positions[player_index].z = player_position.z;
    }
}

void SendPacket(wchar_t ip[], void* data, const int len)
{
    sock.Send(Address(ip, 27016), data, len);
}

void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}

int main(int argc, char **argv)
{
    InitializeSockets();
    
    if (!sock.Open(PORT))
    {
        printf("Failed to open socket on port %d\n", PORT);
        return 1;
    }

    start = std::clock();
    
    while (true)
    {
        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

        if (duration > 15.625)
        {
            Broadcast();
            start = std::clock();
        }

        ReceivePacket();

        CheckPotato();
    }

    ShutdownSockets();

    return 0;
}
