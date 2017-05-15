#define _WINSOCKAPI_
#include <d3d11.h>
#include <xnamath.h>
#include <windowsx.h>

#include "event.h"
#include "global.h"
#include "groundwork.h"
#include "init.h"
#include "load.h"
#include "render.h"
#include "vertex.h"

#define NUM_PLAYERS 6

/*------------------------------------------------------------------------------
Global Variables
*///----------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;


ID3D11Buffer*                       g_pBillboardBuffer = NULL;
ID3D11Buffer*                       g_pSkysphereBuffer = NULL;
ID3D11Buffer*                       g_pCBuffer = NULL;

ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11DepthStencilState			    *ds_on, *ds_off; // states for turning off and on the depth buffer

ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*           g_pMrPTexture = NULL;
ID3D11ShaderResourceView*           g_pMrsPTexture = NULL;
ID3D11ShaderResourceView*           smokeTex = NULL;

// Menu Textures
ID3D11ShaderResourceView*           g_pBLU = NULL;
ID3D11ShaderResourceView*           g_pPlayer1 = NULL;
ID3D11ShaderResourceView*           g_pPlayer2 = NULL;
ID3D11ShaderResourceView*           g_pPlayer3 = NULL;
ID3D11ShaderResourceView*           g_pRED = NULL;
ID3D11ShaderResourceView*           g_pPlayer4 = NULL;
ID3D11ShaderResourceView*           g_pPlayer5 = NULL;
ID3D11ShaderResourceView*           g_pPlayer6 = NULL;

ID3D11ShaderResourceView            *textures[2];

ID3D11SamplerState*                 g_pSamplerLinear = NULL;

ID3D11BlendState* g_BlendState;

XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMFLOAT4                            g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);

camera								cam;
level								level1;
XMFLOAT3							rocket_position;

int model_vertex_anz = 0;
const int NUM_BILLBOARDS = 1;

bool IN_MENU = true;

float Dis[NUM_BILLBOARDS];

Billboard *players[5] = {
    new Billboard(),
    new Billboard(),
    new Billboard(),
    new Billboard(),
    new Billboard()
};

Potato potato;
Target targets[NUM_BILLBOARDS];

/*------------------------------------------------------------------------------
Entry point to the program. Initializes everything and goes into a message
processing loop. Idle time is used to render the scene.
*///----------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!InitNetwork()) return 0;

    if (FAILED(InitWindow(hInstance, nCmdShow))) return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    if (FAILED(InitMenuTextures())) return 0;

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (!Render()) return 0;
        }
    }

    CleanupDevice();

#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif

    return (int)msg.wParam;
}