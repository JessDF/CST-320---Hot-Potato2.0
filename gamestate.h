#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <d3dx11.h>
#include <xnamath.h>

#define MASTER_STATE_SIZE 88
#define PLAYER_STATE_SIZE 28

struct PlayerState {
    int player_index;

    XMFLOAT3 player_position;
    XMFLOAT3 potato_position;
};

struct MasterState {
    int potato_holder;

    XMFLOAT3 player_positions[6];
    XMFLOAT3 potato_position;
};

#endif
