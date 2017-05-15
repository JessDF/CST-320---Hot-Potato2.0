#ifndef INIT_H
#define INIT_H

#include "global.h"
#include "load.h"
#include "socket.h"

#define PORT  27016
#define HEIGHT 480
#define WIDTH 640

Socket sock;

HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint,
    LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    /*
    Setting this flag improves the shader debugging experience, but still
    allows the shaders to be optimized and to run exactly the way they will
    run in the release configuration of this program.
    */
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint,
        szShaderModel, dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
    if (FAILED(hr))
    {
        if (pErrorBlob != NULL)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        }
        if (pErrorBlob) pErrorBlob->Release();
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT InitTexture(LPCWSTR filename, ID3D11ShaderResourceView** srv) {
    return D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, filename,
        NULL, NULL, srv, NULL);
}

HRESULT InitMenuTextures() 
{
    HRESULT hr;
    
    hr = InitTexture(L"assets/menu/BLU.jpeg", &g_pBLU);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player1.jpeg", &g_pPlayer1);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player2.jpeg", &g_pPlayer2);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player3.jpeg", &g_pPlayer3);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/RED.jpeg", &g_pRED);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player4.jpeg", &g_pPlayer4);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player5.jpeg", &g_pPlayer5);
    if (FAILED(hr)) return hr;

    hr = InitTexture(L"assets/menu/Player6.jpeg", &g_pPlayer6);
    if (FAILED(hr)) return hr;

    return hr;
}

/*------------------------------------------------------------------------------
Register class and create window
*///----------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex)) return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"TutorialWindowClass", L"Hot Potato",
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
      rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

    if (!g_hWnd) return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

/*------------------------------------------------------------------------------
Create Direct3D device and swap chain
*///----------------------------------------------------------------------------
HRESULT InitDevice() {
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverType = 0; driverType < numDriverTypes; driverType++) {
        g_driverType = driverTypes[driverType];
        hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
            createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
            &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel,
            &g_pImmediateContext);
        if (SUCCEEDED(hr)) break;
    }
    if (FAILED(hr)) return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
        &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
    if (FAILED(hr)) return hr;


    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV,
        &g_pDepthStencilView);
    if (FAILED(hr)) return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr)) {
        MessageBox(NULL, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
            L"Error", MB_OK);
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
    if (FAILED(hr)) {
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements,
        pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr)) return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr)) {
        MessageBox(NULL, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
            L"Error", MB_OK);
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr)) return hr;

    // Create vertex buffer
    SimpleVertex vertices[] = {
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 12;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pBillboardBuffer);
    if (FAILED(hr)) return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride,
        &offset);


    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBuffer);
    if (FAILED(hr)) return hr;

    LoadCatmullClark(L"assets/ccsphere.cmp", g_pd3dDevice, &g_pSkysphereBuffer,
        &model_vertex_anz);

    hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"assets/mrP.gif", NULL,
        NULL, &g_pMrPTexture, NULL);
    if (FAILED(hr)) return hr;

    hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"assets/mrP.gif", NULL,
        NULL, &g_pMrsPTexture, NULL);
    if (FAILED(hr)) return hr;

    textures[0] = g_pMrPTexture;
    textures[1] = g_pMrsPTexture;

    hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"assets/sky.jpg", NULL,
        NULL, &g_pTextureRV, NULL);
    if (FAILED(hr)) return hr;

    hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"assets/potato.png",
        NULL, NULL, &smokeTex, NULL);
    if (FAILED(hr)) return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
    if (FAILED(hr)) return hr;

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); // camera position
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // look at
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // normal vector on at vector (always up)
    g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height,
        0.01f, 1000.0f);

    ConstantBuffer constantbuffer;
    constantbuffer.view = XMMatrixTranspose(g_View);
    constantbuffer.projection = XMMatrixTranspose(g_Projection);
    constantbuffer.world = XMMatrixTranspose(XMMatrixIdentity());
    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer,
        0, 0);

    //create the depth stencil states for turning the depth buffer on and of:
    D3D11_DEPTH_STENCIL_DESC DS_ON, DS_OFF;
    DS_ON.DepthEnable = true;
    DS_ON.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DS_ON.DepthFunc = D3D11_COMPARISON_LESS;
    // Stencil test parameters
    DS_ON.StencilEnable = true;
    DS_ON.StencilReadMask = 0xFF;
    DS_ON.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing
    DS_ON.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DS_ON.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    DS_ON.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DS_ON.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing
    DS_ON.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DS_ON.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    DS_ON.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DS_ON.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Create depth stencil state
    DS_OFF = DS_ON;
    DS_OFF.DepthEnable = false;
    g_pd3dDevice->CreateDepthStencilState(&DS_ON, &ds_on);
    g_pd3dDevice->CreateDepthStencilState(&DS_OFF, &ds_off);

    level1.init("assets/level.bmp");
    level1.init_texture(g_pd3dDevice, L"assets/coolWall.png");
    level1.init_texture(g_pd3dDevice, L"assets/weird.jpg");
    level1.init_texture(g_pd3dDevice, L"assets/floor.jpg");
    level1.init_texture(g_pd3dDevice, L"assets/ceiling.jpg");

    //srand(NULL);
    for (int i = 0; i < 5; i++) {
        players[i] = new Billboard;
        players[i]->pos.z = players[i]->pos.z + (i * 2);
    }

    //start_client("10.11.120.37", 27015);
}

bool InitNetwork()
{  
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    if(!WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR) return false;
#endif

    return sock.Open(PORT);
}

/*------------------------------------------------------------------------------
Clean up the objects we've created
*///----------------------------------------------------------------------------
void CleanupDevice() {
    if (g_pImmediateContext) g_pImmediateContext->ClearState();
    if (g_pSamplerLinear) g_pSamplerLinear->Release();
    if (g_pTextureRV) g_pTextureRV->Release();
    if (g_pCBuffer) g_pCBuffer->Release();
    if (g_pBillboardBuffer) g_pBillboardBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pDepthStencil) g_pDepthStencil->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}

#endif