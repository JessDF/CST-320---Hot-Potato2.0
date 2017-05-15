#ifndef EVENT_H
#define EVENT_H

#include <windowsx.h>

#include "global.h"

void OnLBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
    if (potato.owner == me) potato.Throw(&cam);
}

// This Function is called every time the Right Mouse Button is down
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) { }

// This Function is called every time a character key is pressed
void OnChar(HWND hwnd, UINT ch, int cRepeat) { }

// This Function is called every time the Left Mouse Button is up
void OnLBU(HWND hwnd, int x, int y, UINT keyFlags) { }

// This Function is called every time the Right Mouse Button is up
void OnRBU(HWND hwnd, int x, int y, UINT keyFlags) { }

// This Function is called every time the Mouse Moves
void OnMM(HWND hwnd, int x, int y, UINT keyFlags) {
    if ((keyFlags & MK_LBUTTON) == MK_LBUTTON) {}
    if ((keyFlags & MK_RBUTTON) == MK_RBUTTON) {}
}

BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct) {
    return TRUE;
}

void OnTimer(HWND hwnd, UINT id) { }

void OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
    switch (vk) {
    case 32: // Space
        break;

    case 37: // Left
        players[1]->imp.x = 0;
        break;

    case 38: // Up
        players[1]->imp.z = 0;
        break;

    case 39: // Right
        players[1]->imp.x = 0;
        break;

    case 40: // Down
        players[1]->imp.z = 0;
        break;

    case 65: // A
        cam.a = 0;
        break;

    case 68: // D
        cam.d = 0;
        break;

    case 73: // I
        players[0]->imp.z = 0;
        break;

    case 74: // J
        players[0]->imp.x = 0;
        break;

    case 75: // K
        players[0]->imp.z = 0;
        break;

    case 76: // L
        players[0]->imp.x = 0;
        break;

    case 83: // S
        cam.s = 0;
        break;

    case 87: // W
        cam.w = 0;
        break;

    default:
        break;
    }
}

void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
    switch (vk) {
    case 27:
        PostQuitMessage(0);
        break;

    case 32: // Space
        if (active_players == 6) menu = false;
        break;

    case 37: // Left
        players[1]->imp.x -= .1;
        break;

    case 38: // Up
        players[1]->imp.z += .1;
        break;

    case 39: // Right
        players[1]->imp.x += .1;
        break;

    case 40: // Down
        players[1]->imp.z -= .1;
        break;

    case 65: // A
        cam.a = 1;
        break;

    case 68: // D
        cam.d = 1;
        break;

    case 73: // I
        players[0]->imp.z += .1;
        break;

    case 74: // J
        players[0]->imp.x -= .1;
        break;

    case 75: // K
        players[0]->imp.z -= .1;
        break;

    case 76: // L
        players[0]->imp.x += .1;
        break;

    case 83: // S
        cam.s = 1;
        break;

    case 87: // W
        cam.w = 1;
        break;

    default:
        break;
    }
}


/*------------------------------------------------------------------------------
Called every time the application receives a message
*///----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLBD);
        HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLBU);
        HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMM);
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
        HANDLE_MSG(hWnd, WM_KEYDOWN, OnKeyDown);
        HANDLE_MSG(hWnd, WM_KEYUP, OnKeyUp);

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

#endif