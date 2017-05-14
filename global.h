#ifndef GLOBAL_H
#define GLOBAL_H

#include <d3dx11.h>
#include <xnamath.h>

#include "groundwork.h"

extern HINSTANCE                           g_hInst;
extern HWND                                g_hWnd;

extern D3D_DRIVER_TYPE                     g_driverType;
extern D3D_FEATURE_LEVEL                   g_featureLevel;

extern ID3D11Device*                       g_pd3dDevice;
extern ID3D11DeviceContext*                g_pImmediateContext;
extern IDXGISwapChain*                     g_pSwapChain;
extern ID3D11RenderTargetView*             g_pRenderTargetView;
extern ID3D11Texture2D*                    g_pDepthStencil;
extern ID3D11VertexShader*                 g_pVertexShader;
extern ID3D11PixelShader*                  g_pPixelShader;
extern ID3D11InputLayout*                  g_pVertexLayout;

extern ID3D11Buffer*                       g_pBillboardBuffer;
extern ID3D11Buffer*                       g_pSkysphereBuffer;
extern ID3D11Buffer*                       g_pCBuffer;

extern ID3D11DepthStencilView*             g_pDepthStencilView;
extern ID3D11DepthStencilState			   *ds_on, *ds_off;

// Potato Character textures
extern ID3D11ShaderResourceView*           g_pMrPTexture;
extern ID3D11ShaderResourceView*           g_pMrsPTexture;

extern ID3D11ShaderResourceView*           smokeTex;
extern ID3D11ShaderResourceView*           g_pTextureRV;

// Menu Textures
extern ID3D11ShaderResourceView*           g_pBLU;
extern ID3D11ShaderResourceView*           g_pPlayer1;
extern ID3D11ShaderResourceView*           g_pPlayer2;
extern ID3D11ShaderResourceView*           g_pPlayer3;
extern ID3D11ShaderResourceView*           g_pRED;
extern ID3D11ShaderResourceView*           g_pPlayer4;
extern ID3D11ShaderResourceView*           g_pPlayer5;
extern ID3D11ShaderResourceView*           g_pPlayer6;

extern ID3D11ShaderResourceView            *textures[2];

extern ID3D11SamplerState*                 g_pSamplerLinear;

extern ID3D11BlendState* g_BlendState;

extern XMMATRIX                            g_World;
extern XMMATRIX                            g_View;
extern XMMATRIX                            g_Projection;
extern XMFLOAT4                            g_vMeshColor;

extern camera							   cam;
extern level							   level1;
extern XMFLOAT3							   rocket_position;

extern int model_vertex_anz;
extern const int NUM_BILLBOARDS;

extern bool IN_MENU;

extern float Dis[1];

extern Billboard *billboards[1];
extern XMFLOAT4 positions[1];

extern Target targets[];

extern Potato potato;

extern XMFLOAT3 menu_position[];
extern XMFLOAT3 menu_position_shown[];
extern XMMATRIX menu_scaling[];
extern XMMATRIX menu_scaling_shown[];

#endif