#include <float.h>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include "socket.h"
#include <stack>
#include <stdlib.h>
#include <wchar.h>

#define DEFAULT_PORT 27015
#define NUM_PLAYERS 6
#define NUM_POSITIONS 21
#define PACKET_SIZE 6
#define D_SCL_SECURE_NO_WARNINGS

double all_positions[NUM_POSITIONS];
std::mutex lock;
wchar_t ips[NUM_PLAYERS][16];
Socket sock;
std::stack<int> player_slots;

struct GameState
{
    wchar_t ips[NUM_PLAYERS][16];
    double all_positions[NUM_POSITIONS];
};
GameState game_state;

void SendPacket(wchar_t ip[], void* data, const int len); 

void CALLBACK Broadcast() {
    lock.lock();
    
    wchar_t ips_temp[NUM_PLAYERS][16];
    std::copy(&ips[0][0], &ips[0][0] + 16 * 6, &ips_temp[0][0]);
    
    double all_positions_temp[NUM_POSITIONS];
    std::copy(all_positions, all_positions + NUM_POSITIONS, all_positions_temp);

    lock.unlock();

    for (int i = 0; i < NUM_PLAYERS; i++) {
        SendPacket(ips_temp[i], all_positions_temp, sizeof(float) * NUM_POSITIONS);
    }
}

void CreateSocket()
{
    if (!sock.Open(DEFAULT_PORT)) printf("failed to open socket!\n");
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

int IPConnected(wchar_t ip[]) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (wcscmp(ip, ips[i]) == 0) return i;
    }
    return -1;
}

void ReceivePackets()
{
    while (true)
    {
        Address sender;
        float positions[PACKET_SIZE];
        int bytes_read = sock.Receive(sender, positions, sizeof(positions));
        if (bytes_read <= 0) continue;

        // process packet
        wchar_t addr[16];
        sender.GetAddress(addr);
        int index = IPConnected(addr);
        if (index < 0 && player_slots.size() > 0) {
            index = player_slots.top();
            player_slots.pop();
            wcsncpy_s(ips[index], 16, addr, 16);
        }
        index *= 3;
        all_positions[index] = positions[0];
        all_positions[index + 1] = positions[1];
        all_positions[index + 2] = positions[2];
        if (positions[3] != -FLT_MAX) {
            all_positions[18] = positions[3];
            all_positions[19] = positions[4];
            all_positions[20] = positions[5];
        }

        printf("%s %f %f %f %f %f %f\n", addr, positions[0], positions[1], positions[2],
            positions[3], positions[4], positions[5]);
        printf("%f %f %f\n", all_positions[0], all_positions[1],
            all_positions[2]);
        printf("%f %f %f\n", all_positions[3], all_positions[4],
            all_positions[5]);
        printf("%f %f %f\n", all_positions[6], all_positions[7],
            all_positions[8]);
        printf("%f %f %f\n", all_positions[9], all_positions[10],
            all_positions[11]);
        printf("%f %f %f\n", all_positions[12], all_positions[13],
            all_positions[14]);
        printf("%f %f %f\n", all_positions[15], all_positions[16],
            all_positions[17]);
        printf("potato %f %f %f\n\n", all_positions[18], all_positions[19],
            all_positions[20]);
    }
}

void Run() {
    ReceivePackets();
}

void SendPacket(wchar_t ip[], void* data, const int len)
{
    sock.Send(Address(ip, 27015), data, len);
}

void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}

int main(int argc, char **argv)
{
    player_slots.push(5);
    player_slots.push(4);
    player_slots.push(3);
    player_slots.push(2);
    player_slots.push(1);
    player_slots.push(0);

    InitializeSockets();
    CreateSocket();

    SetTimer(NULL, 0, 64, (TIMERPROC)&Broadcast);

    Run();

    ShutdownSockets();
    return 0;
}
