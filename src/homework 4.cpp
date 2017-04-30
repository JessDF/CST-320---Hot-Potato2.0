#include "groundwork.h"
// breaks code at the moment
/*
#include "Net.h"

using namespace std;
using namespace net;
*/

/*------------------------------------------------------------------------------
    Structures
*///----------------------------------------------------------------------------
struct CatmullVertex {
  XMFLOAT3 pos;
  XMFLOAT3 normal;
  XMFLOAT2 tex;
};

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
ID3D11DepthStencilState			        *ds_on, *ds_off; //states for turning off and on the depth buffer

ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*           g_pMrPTexture = NULL;
ID3D11ShaderResourceView*           g_pMrsPTexture = NULL;

ID3D11ShaderResourceView            *textures[2];

ID3D11SamplerState*                 g_pSamplerLinear = NULL;

XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMFLOAT4                            g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );

camera								cam;
level								level1;

int model_vertex_anz = 0;
const int NUM_BILLBOARDS = 2;

ID3D11BlendState*					g_BlendState;

Billboard *billboards[NUM_BILLBOARDS];
XMFLOAT4 positions[NUM_BILLBOARDS];
/*
vector<Address> addresses;
Socket socket;
bool position_changed;
*/
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();
bool LoadCatmullClark(LPCTSTR filename, ID3D11Device* g_pd3dDevice, ID3D11Buffer **ppVertexBuffer, int *vertex_count);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
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

  D3D_DRIVER_TYPE driverTypes[] =
  {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] =
  {
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

  for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    g_driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
      D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
    if (SUCCEEDED(hr))
      break;
  }
  if (FAILED(hr))
    return hr;

  // Create a render target view
  ID3D11Texture2D* pBackBuffer = NULL;
  hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
  if (FAILED(hr))
    return hr;

  hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
  pBackBuffer->Release();
  if (FAILED(hr))
    return hr;

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
  hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
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
  if (FAILED(hr))
  {
    MessageBox(NULL,
      L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
    return hr;
  }

  // Create the vertex shader
  hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
  if (FAILED(hr))
  {
    pVSBlob->Release();
    return hr;
  }

  // Define the input layout
  D3D11_INPUT_ELEMENT_DESC layout[] =
  {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  UINT numElements = ARRAYSIZE(layout);

  // Create the input layout
  hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
    pVSBlob->GetBufferSize(), &g_pVertexLayout);
  pVSBlob->Release();
  if (FAILED(hr)) return hr;

  // Set the input layout
  g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

  // Compile the pixel shader
  ID3DBlob* pPSBlob = NULL;
  hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
  if (FAILED(hr))
  {
    MessageBox(NULL,
      L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
    return hr;
  }

  // Create the pixel shader
  hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
  pPSBlob->Release();
  if (FAILED(hr)) return hr;

  if (FAILED(hr)) return hr;

  // Create vertex buffer
  SimpleVertex vertices[] = {
    { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },

    { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }
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
  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride, &offset);


  // Set primitive topology
  g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Create the constant buffers
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(ConstantBuffer);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBuffer);
  if (FAILED(hr)) return hr;

  LoadCatmullClark(L"ccsphere.cmp", g_pd3dDevice, &g_pSkysphereBuffer, &model_vertex_anz);

  hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"mrP.gif", NULL, NULL, &g_pMrPTexture, NULL);
  if (FAILED(hr)) return hr;

  hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"MRSP.png", NULL, NULL, &g_pMrsPTexture, NULL);
  if (FAILED(hr)) return hr;

  textures[0] = g_pMrPTexture;
  textures[1] = g_pMrsPTexture;

  hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"sky.jpg", NULL, NULL, &g_pTextureRV, NULL);
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
  XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -6.0f, 0.0f);//camera position
  XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);//look at
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);// normal vector on at vector (always up)
  g_View = XMMatrixLookAtLH(Eye, At, Up);

  // Initialize the projection matrix
  g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f);

  ConstantBuffer constantbuffer;
  constantbuffer.View = XMMatrixTranspose(g_View);
  constantbuffer.Projection = XMMatrixTranspose(g_Projection);
  constantbuffer.World = XMMatrixTranspose(XMMatrixIdentity());
  g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0);

  //create the depth stencil states for turning the depth buffer on and of:
  D3D11_DEPTH_STENCIL_DESC		DS_ON, DS_OFF;
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

  level1.init("level(newest).bmp");
  level1.init_texture(g_pd3dDevice, L"coolWall.png");
  level1.init_texture(g_pd3dDevice, L"weird.jpg");
  level1.init_texture(g_pd3dDevice, L"floor.jpg");
  level1.init_texture(g_pd3dDevice, L"ceiling.jpg");

  for (int i = 0; i < NUM_BILLBOARDS; i++) {
    billboards[i] = new Billboard;
  }



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



  // need to fix Net.h errors
  /*
  // initialize networking
  if (!InitializeSockets()) {
    printf("failed to intitialize sockets\n");
    return 1;
  }

  // create scoket
  int port = 3000;
  printf("creating socket on port %d\n", port);

  if (!socket.Open(port)) {
    printf("creating socket on port %d\n", port + 1);
    if (!socket.Open(port)) {
      printf("failed to create socket!\n");
      return 1;
    }
  }

  addresses.push_back(Address(10, 0, 0, 14, port));
  */
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pSamplerLinear ) g_pSamplerLinear->Release();
    if( g_pTextureRV ) g_pTextureRV->Release();
    if(g_pCBuffer) g_pCBuffer->Release();
    if( g_pBillboardBuffer ) g_pBillboardBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

// This Function is called every time the Left Mouse Button is down
void OnLBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
  //bull = new bullet;
  //bull->pos.x = -cam.position.x;
  //bull->pos.y = -cam.position.y - 0.2;
  //bull->pos.z = -cam.position.z;
  //XMMATRIX CR = XMMatrixRotationY(-cam.rotation.y);

  //XMFLOAT3 forward = XMFLOAT3(0, 0, 3);
  //XMVECTOR f = XMLoadFloat3(&forward);
  //f = XMVector3TransformCoord(f, CR);
  //XMStoreFloat3(&forward, f);

  //bull->imp = forward;
  //static float m1 = 3; 
  //m1 = rand() % 3; 
  // bull = new bullet;
  // bull->pos.x = m1; 
  // bull->pos.y = 0;
  // bull->pos.z = 15;


  // p1 = new bullet; 
  // p1->pos.x = 2;
  // p1->pos.y = 0;
  // p1->pos.z = 10;


}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is down
///////////////////////////////////
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
  {

  }
///////////////////////////////////
//		This Function is called every time a character key is pressed
///////////////////////////////////
void OnChar(HWND hwnd, UINT ch, int cRepeat)
  {

  }
///////////////////////////////////
//		This Function is called every time the Left Mouse Button is up
///////////////////////////////////
void OnLBU(HWND hwnd, int x, int y, UINT keyFlags)
  {


  }
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is up
///////////////////////////////////
void OnRBU(HWND hwnd, int x, int y, UINT keyFlags)
  {


  }
///////////////////////////////////
//		This Function is called every time the Mouse Moves
///////////////////////////////////
void OnMM(HWND hwnd, int x, int y, UINT keyFlags)
  {
  if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
    {
    }

  if ((keyFlags & MK_RBUTTON) == MK_RBUTTON)
    {
    }
  
  }


BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
  {

  return TRUE;
  }
void OnTimer(HWND hwnd, UINT id)
  {

  }
//*************************************************************************
void OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
  switch (vk) {
    case 32: // Space
      break;

    case 37: // Left
      billboards[1]->imp.x = 0;
      break;

    case 38: // Up
      billboards[1]->imp.z = 0;
      break;

    case 39: // Right
      billboards[1]->imp.x = 0;
      break;

    case 40: // Down
      billboards[1]->imp.z = 0;
      break;

    case 65: // A
      cam.a = 0;
      break;

    case 68: // D
      cam.d = 0;
      break;

    case 73: // I
      billboards[0]->imp.z = 0;
      break;

    case 74: // J
      billboards[0]->imp.x = 0;
      break;

    case 75: // K
      billboards[0]->imp.z = 0;
      break;

    case 76: // L
      billboards[0]->imp.x = 0;
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
  // position_changed = true;
  switch (vk) {
    case 27:
      PostQuitMessage(0);
      break;

    case 32: // Space
      break;

    case 37: // Left
      billboards[1]->imp.x -= .1;
      break;

    case 38: // Up
      billboards[1]->imp.z += .1;
      break;

    case 39: // Right
      billboards[1]->imp.x += .1;
      break;

    case 40: // Down
      billboards[1]->imp.z -= .1;
      break;

    case 65: // A
      cam.a = 1;
      break;

    case 68: // D
      cam.d = 1;
      break;

    case 73: // I
      billboards[0]->imp.z += .1;
      break;

    case 74: // J
      billboards[0]->imp.x -= .1;
      break;

    case 75: // K
      billboards[0]->imp.z -= .1;
      break;

    case 76: // L
      billboards[0]->imp.x += .1;
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

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
#include <windowsx.h>
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
  HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLBD);
  HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLBU);
  HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMM);
  HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
  HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
  HANDLE_MSG(hWnd, WM_KEYDOWN, OnKeyDown);
  HANDLE_MSG(hWnd, WM_KEYUP, OnKeyUp);
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

bool LoadCatmullClark(LPCTSTR filename, ID3D11Device* g_pd3dDevice, 
                      ID3D11Buffer **ppVertexBuffer, int *vertex_count) {
  HANDLE file;
  std::vector<SimpleVertex> data;
  DWORD burn;

  file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL, NULL);

  SetFilePointer(file, 80, NULL, FILE_BEGIN);
  ReadFile(file, vertex_count, 4, &burn, NULL);

  for (int i = 0; i < *vertex_count; ++i) {
    CatmullVertex vertData;
    ReadFile(file, &vertData, sizeof(CatmullVertex), &burn, NULL);
    SimpleVertex sv;
    sv.Pos = vertData.pos;
    sv.Norm = vertData.normal;
    sv.Tex = vertData.tex;
    data.push_back(sv);
  }

  D3D11_BUFFER_DESC desc = {
    sizeof(SimpleVertex)* *vertex_count,
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_VERTEX_BUFFER,
    0, 0,
    sizeof(SimpleVertex)
  };

  D3D11_SUBRESOURCE_DATA subdata = {
    &(data[0]), 0, 0
  };

  HRESULT hr = g_pd3dDevice->CreateBuffer(&desc, &subdata, ppVertexBuffer);
  return !FAILED(hr);
}

void DrawBillboard(int billboard_index, long elapsed, XMMATRIX &view,
                   ConstantBuffer &constantbuffer) {
  XMMATRIX world = billboards[billboard_index]->GetMatrix(elapsed, view);

  constantbuffer.World = XMMatrixTranspose(world);
  constantbuffer.View = XMMatrixTranspose(view);
  constantbuffer.Projection = XMMatrixTranspose(g_Projection);
  constantbuffer.info.x = billboards[billboard_index]->pos.x;
  constantbuffer.info.y = billboards[billboard_index]->pos.y;
  constantbuffer.info.z = billboards[billboard_index]->pos.z;

  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
  g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
  g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
  g_pImmediateContext->PSSetShaderResources(0, 1, &textures[billboard_index]);
  g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0);

  g_pImmediateContext->Draw(12, 0);
}


/*------------------------------------------------------------------------------
    Render a frame
*///----------------------------------------------------------------------------
void Render() {
  static StopWatchMicro_ stopwatch;
  long elapsed = stopwatch.elapse_micro();
  stopwatch.start();//restart
  
  // Nearly working
  /*
  Address sender;
  unsigned char buffer[256];
  int bytes_read = socket.Receive(sender, buffer, sizeof(buffer));
  unsigned char *hold;
  unsigned char *move;
  unsigned char *positions[6];
  int i = 0;
  if (bytes_read) {
    move = buffer;
    while (*move != '\0') {
      hold = move;
      while (*move != 'x') {
        move++;
      }
      *move = '\0';
      move++;
      positions[i++] = hold;
      hold = move;
    }
  }
  
  for (int i = 0; i < 6; i++) {
    printf("%s\n", positions[i]);
  }
  */
  UINT stride = sizeof(SimpleVertex);
  UINT offset = 0;

  static float m1 = 3, imp;
  m1 = rand() % 3;
  imp = .05; 
  
  // Update our time
  static float t = 0.0f;
  t += 0.001;

  cam.animation(level1.get_bitmap());
  XMMATRIX view = cam.get_matrix(&g_View);

  XMMATRIX worldmatrix;
  worldmatrix = XMMatrixIdentity();
    
  float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
  g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
    
  // Clear the depth buffer to 1.0 (max depth)
  g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  ConstantBuffer constantbuffer;

  int max_x = 5;
  int max_z = 5;
 

  // Update skybox constant buffer
  g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
  constantbuffer.World = XMMatrixTranspose(XMMatrixScaling(1, 1, 1));
  constantbuffer.View = XMMatrixTranspose(view);
  constantbuffer.Projection = XMMatrixTranspose(g_Projection);
  constantbuffer.info = XMFLOAT4(0, 0, 0, 0);
  g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0);

  //render all the walls of the level
  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pBillboardBuffer, &stride, &offset);
  level1.render_level(g_pImmediateContext, g_pBillboardBuffer, &view, &g_Projection, g_pCBuffer);


  for (int i = 0; i < NUM_BILLBOARDS; i++) {
	  DrawBillboard(i, elapsed, view, constantbuffer);
  }


  // Present our back buffer to our front buffer
  g_pSwapChain->Present(0, 0);
}
