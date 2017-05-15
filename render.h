#ifndef RENDER_H
#define RENDER_H

#include <d3d11.h>
#include <xnamath.h>

#include "global.h"
#include "potato.h"

long elapsed;

UINT offset = 0;
UINT stride = sizeof(SimpleVertex);

bool was_owner = false;

XMFLOAT3 menu_position[] = {
    XMFLOAT3(-0.00405f, 0.002f, 0.01f),
    XMFLOAT3(-0.00405f, 0.001f, 0.01f),
    XMFLOAT3(-0.00405f, 0.0f, 0.01f),
    XMFLOAT3(0.00405f, 0.002f, 0.01f),
    XMFLOAT3(0.00405f, 0.001f, 0.01f),
    XMFLOAT3(0.00405f, 0.0f, 0.01f),
    XMFLOAT3(-0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(0.0035f, 0.0032f, 0.01f)
};

XMFLOAT3 menu_position_hidden[] = {
    XMFLOAT3(-0.0124f, 0.002f, 0.01f),
    XMFLOAT3(-0.0124f, 0.001f, 0.01f),
    XMFLOAT3(-0.0124f, 0.0f, 0.01f),
    XMFLOAT3(0.0124f, 0.002f, 0.01f),
    XMFLOAT3(0.0124f, 0.001f, 0.01f),
    XMFLOAT3(0.0124f, 0.0f, 0.01f),
    XMFLOAT3(-0.0035f, 0.0032f, 0.01f),
    XMFLOAT3(0.0035f, 0.0032f, 0.01f),
};

XMMATRIX menu_scaling[] = { 
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.0015, .0003, .001),
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.002, .0005, .001),
}; 

XMMATRIX menu_scaling_hidden[] = {
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.0075, .0003, .001),
    XMMatrixScaling(0.002, .0005, .001),
    XMMatrixScaling(0.002, .0005, .001),
};

bool menu_shown[] = { false, false, false, false, false, false, true, true };

void RenderGame() {
    ConstantBuffer constant_buffer = ConstantBuffer();
    XMMATRIX aoffset, projection, Ry, scaling, view, world;
    
    cam.animation(level1.get_bitmap());

    potato.Update(elapsed, level1.get_bitmap(), players, NUM_PLAYERS);

    world = potato.getWorldMatrix(&cam);
    Ry = XMMatrixRotationY(-cam.rotation.y);
    scaling = XMMatrixScaling(0.1, 0.1, 0.1);
    aoffset = XMMatrixTranslation(0, 0, -.001);
    world = scaling * aoffset * Ry * world;

    view = cam.get_matrix(&g_View);

    projection = XMMatrixTranspose(g_Projection);

    constant_buffer.world = XMMatrixTranspose(world);
    constant_buffer.view = XMMatrixTranspose(view);
    constant_buffer.projection = XMMatrixTranspose(g_Projection);
    
    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->PSSetShaderResources(0, 1, &smokeTex);
    g_pImmediateContext->VSSetShaderResources(0, 1, &smokeTex);
    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, 
      &constant_buffer, 0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride,
        &offset);

    g_pImmediateContext->Draw(12, 0);

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
    
    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer1);
    SetMenuItem(0);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer2);
    SetMenuItem(1);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer3);
    SetMenuItem(2);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer4);
    SetMenuItem(3);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer5);
    SetMenuItem(4);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pPlayer6);
    SetMenuItem(5);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pBLU);
    SetMenuItem(6);
    g_pImmediateContext->Draw(12, 0);

    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pRED);
    SetMenuItem(7);
    g_pImmediateContext->Draw(12, 0);
}

bool RenderPotatoes() {
    ConstantBuffer constant_buffer = ConstantBuffer();
    float state[23];
    Address sender;
    int bytes_read = sock.Receive(sender, state, 92);

    int start;
    if (bytes_read > 0) {
        me = (int)state[21];
        owner = (int)state[22];
        if (owner == me) {
            was_owner = true;
        } else {
            potato.pos.x = state[18];
            potato.pos.y = state[19];
            potato.pos.z = state[20];

            potato.owner = state[22];
        }

        menu_shown[me] = true;
        for (int i = 0; i < 5; i++) {
            start = i * 3;
            menu_shown[i] = !(state[start] == FLT_MAX);
            if (i != me) 
            {
                players[i]->pos.x = state[start + 0];
                players[i]->pos.y = state[start + 1];
                players[i]->pos.z = state[start + 2];
            }
            else 
            {
                players[i]->pos.x = cam.position.x;
                players[i]->pos.y = cam.position.y;
                players[i]->pos.z = cam.position.z;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        if (i != me && me < 6) {
            XMMATRIX view = cam.get_matrix(&XMMatrixIdentity());
            XMMATRIX world = players[i]->GetMatrix(elapsed, view);
            constant_buffer.world = XMMatrixTranspose(world);
            constant_buffer.view = XMMatrixTranspose(view);
            constant_buffer.projection = XMMatrixTranspose(g_Projection);

            g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
            g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
            g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
            g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
            g_pImmediateContext->PSSetShaderResources(0, 1, &g_pMrPTexture);
            g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constant_buffer, 0, 0);

            g_pImmediateContext->Draw(12, 0);
        }
    }

    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride, &offset);


    return true;
}

bool Render() 
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

    float blendFactor[] = { 0, 0, 0, 0 };
    UINT sampleMask = 0xffffffff;
    g_pImmediateContext->OMSetBlendState(g_BlendState, blendFactor, sampleMask);

    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
    g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

    RenderGame();
    RenderLevel();
    if (menu) RenderMenu();
    if (!RenderPotatoes()) return false;

    float potx = potato.pos.x;
    if (!was_owner) potx = FLT_MAX;
    was_owner = false;

    g_pSwapChain->Present(0, 0);
    float data[] = {
        -cam.position.x,
        -cam.position.y,
        -cam.position.z,
        potx,
        potato.pos.y,
        potato.pos.z,
        potato.owner,
    };
    sock.Send(Address(L"10.0.0.8", 27015), data, 28);
    return true;
}

#endif