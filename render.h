#ifndef RENDER_H
#define RENDER_H

#include <d3d11.h>
#include <xnamath.h>

#include "global.h"

long elapsed;

UINT offset = 0;
UINT stride = sizeof(SimpleVertex);

XMFLOAT3 menu_position[] = {
    XMFLOAT3(-0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(-0.00405f, 0.002f, 0.01f),
    XMFLOAT3(-0.00405f, 0.001f, 0.01f),
    XMFLOAT3(-0.00405f, 0.0f, 0.01f),
    XMFLOAT3(0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(0.00405f, 0.002f, 0.01f),
    XMFLOAT3(0.00405f, 0.001f, 0.01f),
    XMFLOAT3(0.00405f, 0.0f, 0.01f)
};

XMFLOAT3 menu_position_hidden[] = {
    XMFLOAT3(-0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(-0.0124f, 0.002f, 0.01f),
    XMFLOAT3(-0.0124f, 0.001f, 0.01f),
    XMFLOAT3(-0.0124f, 0.0f, 0.01f),
    XMFLOAT3(0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(0.0124f, 0.002f, 0.01f),
    XMFLOAT3(0.0124f, 0.001f, 0.01f),
    XMFLOAT3(0.0124f, 0.0f, 0.01f)
};

XMMATRIX menu_scaling[] = { 
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001)
}; 

XMMATRIX menu_scaling_hidden[] = {
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001)
};

bool menu_shown[] = { true, false, true, false, true, false, true, true };


//XMMATRIX view;
//XMMATRIX world;

void RenderGame() {
    XMMATRIX projection = XMMatrixTranspose(g_Projection);
    ConstantBuffer constant_buffer = ConstantBuffer();
    cam.animation(level1.get_bitmap());
    XMMATRIX view = cam.get_matrix(&g_View);

    for (int i = 0; i < NUM_BILLBOARDS; i++) {
        Dis[i] = billboards[i]->distanceZ(view);
    }

    sort(begin(Dis), end(Dis));

    static billboard bill;
    bill.position = rocket_position;
    XMMATRIX world = bill.get_matrix(view);
    XMMATRIX Ry = XMMatrixRotationY(-cam.rotation.y);
    XMMATRIX scaling = XMMatrixScaling(0.1, 0.1, 0.1);

    potato.Update(elapsed, level1.get_bitmap(), targets, NUM_BILLBOARDS);
    world = potato.getWorldMatrix(&cam, targets);
    world = scaling * Ry * world;

    constant_buffer.world = XMMatrixTranspose(world);
    constant_buffer.view = XMMatrixTranspose(view);
    constant_buffer.projection = XMMatrixTranspose(g_Projection);
    constant_buffer.info = XMFLOAT4(1, 1, 1, 1);
    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->PSSetShaderResources(0, 1, &smokeTex);
    g_pImmediateContext->VSSetShaderResources(0, 1, &smokeTex);
    g_pImmediateContext->PSSetShaderResources(1, 1, &g_pTextureRV);
    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constant_buffer,
        0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);

    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride,
        &offset);

    g_pImmediateContext->Draw(12, 0);

    //blendstate:
    D3D11_BLEND_DESC blendStateDesc;
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
    blendStateDesc.AlphaToCoverageEnable = TRUE;
    blendStateDesc.IndependentBlendEnable = FALSE;
    blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    g_pd3dDevice->CreateBlendState(&blendStateDesc, &g_BlendState);


    float blendFactor[] = { 0, 0, 0, 0 };
    UINT sampleMask = 0xffffffff;
    g_pImmediateContext->OMSetBlendState(g_BlendState, blendFactor, sampleMask);

    g_pImmediateContext->OMSetDepthStencilState(ds_on, 1);
}

void RenderLevel()
{
    XMMATRIX view = cam.get_matrix(&g_View);
    //render all the walls of the level
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride,
        &offset);
    level1.render_level(g_pImmediateContext, g_pBillboardBuffer, &view,
        &g_Projection, g_pCBuffer);
}

void SetMenuItem(int index)
{
    ConstantBuffer constant_buffer;
    XMMATRIX view, world;

    constant_buffer = ConstantBuffer();
   
    world = XMMatrixRotationY(-cam.rotation.y);
    world = menu_shown[index] ? 
        menu_scaling[index] : 
        menu_scaling_hidden[index];
    world *= XMMatrixRotationY(-cam.rotation.y);
    constant_buffer.world = XMMatrixTranspose(world);
    
    view = cam.get_matrix(&XMMatrixIdentity());
    if (menu_shown[index]) 
    {
        view._41 = menu_position[index].x;
        view._42 = menu_position[index].y;
        view._43 = menu_position[index].z;
    }
    else
    {
        view._41 = menu_position_hidden[index].x;
        view._42 = menu_position_hidden[index].y;
        view._43 = menu_position_hidden[index].z;
    }
    constant_buffer.view = XMMatrixTranspose(view);

    constant_buffer.projection = XMMatrixTranspose(g_Projection);

    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constant_buffer,
        0, 0);
}

void RenderMenu() 
{
    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pBLU);
    SetMenuItem(0); 
    g_pImmediateContext->Draw(12, 0);
    
    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer1);
    SetMenuItem(1);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer2);
    SetMenuItem(2);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer3);
    SetMenuItem(3);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pRED);
    SetMenuItem(4);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer4);
    SetMenuItem(5);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer5);
    SetMenuItem(6);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer6);
    SetMenuItem(7);
    g_pImmediateContext->Draw(12, 0);
}

void Render() 
{
    static StopWatchMicro_ stopwatch;
    elapsed = stopwatch.elapse_micro();
    stopwatch.start();//restart

    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView,
        D3D11_CLEAR_DEPTH, 1.0f, 0);

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView,
        g_pDepthStencilView);

    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

    RenderGame();
    RenderLevel();
    RenderMenu();

    g_pSwapChain->Present(0, 0);
}

#endif