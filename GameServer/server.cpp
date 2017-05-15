#include <float.h>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include "socket.h"
#include <stack>
#include <stdlib.h>
#include <thread>
#include <wchar.h>
#include <windows.h>

#define DEFAULT_PORT 27015
#define IP_LENGTH 16
#define NUM_PLAYERS 6
#define NUM_POSITIONS 22
#define PACKET_SIZE 6
#define D_SCL_SECURE_NO_WARNINGS

float all_positions[NUM_POSITIONS] = {
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    FLT_MAX,
    0.0f
};

std::mutex lock;
wchar_t ips[NUM_PLAYERS][IP_LENGTH];
Socket sock;
std::stack<int> player_slots;

MSG Msg;

struct GameState
{
    wchar_t ips[NUM_PLAYERS][IP_LENGTH];
    double all_positions[NUM_POSITIONS];
};
GameState game_state;

void SendPacket(wchar_t ip[], void* data, const int len); 

void Broadcast() {
    UINT TimerId = (UINT)SetTimer(NULL, 0, 0, NULL); // 0 minute

    while (true)
    {
        GetMessage(&Msg, NULL, 0, 0);

        if (Msg.message == WM_TIMER)
        {
            KillTimer(NULL, TimerId);

            lock.lock();

            wchar_t ips_temp[NUM_PLAYERS][IP_LENGTH];
            std::copy(&ips[0][0], &ips[0][0] + IP_LENGTH * 6, &ips_temp[0][0]);

            float all_positions_temp[NUM_POSITIONS + 1];
            std::copy(all_positions, all_positions + NUM_POSITIONS, all_positions_temp);

            lock.unlock();

            for (int i = 0; i < NUM_PLAYERS; i++) {
                all_positions_temp[21] = i;
                SendPacket(ips_temp[i], all_positions_temp, 88);
            }

            TimerId = (UINT)SetTimer(NULL, 0, 15, NULL);
        }
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

float  positions2[] = {
    -2.0f,
    0.0f,
    22.0f,
    FLT_MAX,
    rand() % 3 + 3,
    rand() % 3 + 3,
};

void ProcessPacket(wchar_t addr[], float positions[]) {
    int index = IPConnected(addr);
    if (index < 0 && player_slots.size() > 0) {
        index = player_slots.top();
        player_slots.pop();
        wcsncpy_s(ips[index], IP_LENGTH, addr, IP_LENGTH);
    }
    index *= 3;
    all_positions[index] = positions[0];
    all_positions[index + 1] = positions[1];
    all_positions[index + 2] = positions[2];

    if (positions[3] != FLT_MAX) {
        all_positions[18] = positions[3];
        all_positions[19] = positions[4];
        all_positions[20] = positions[5];
        all_positions[22] = positions[6];

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

void ReceivePackets()
{
    while (true) {
        float positions[PACKET_SIZE];

        Address sender;
        int bytes_read = sock.Receive(sender, positions, sizeof(positions));
        if (bytes_read <= 0) continue;

        // process packet
        wchar_t addr[IP_LENGTH];
        sender.GetAddress(addr);
        ProcessPacket(addr, positions);
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
    player_slots.push(5);
    player_slots.push(4);
    player_slots.push(3);
    player_slots.push(2);
    player_slots.push(1);
    player_slots.push(0);

    InitializeSockets();
    CreateSocket();

    std::thread broadcast(Broadcast);
    std::thread run(ReceivePackets);

    run.join();
    broadcast.join();

    ShutdownSockets();
    return 0;
}
