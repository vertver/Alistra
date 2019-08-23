#include "Base.h"
#include "Base_Window.h"
#include "Base_Render.h"
#include <cstdio>

#include <d3d11.h>
#include <d3dcompiler.h>

#define _XM_NO_INTRINSICS_ 
#include <DirectXMath.h>
#include <cmath>

__declspec(align(16)) typedef struct
{
    DirectX::XMFLOAT2 resolution;
    float time;
} ComputeConstantBuffer;



//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

float BackGroundTest[] = { 0.1f, 0.1f, 0.1f, 1.0f };

DEFINE_IID(ID3D11Texture2D, 6f15aaf2, d208, 4e89, 9a, b4, 48, 95, 35, d3, 4f, 9c);

// it is never ImGui
bool bImgui = false;
bool bVSync = true;
bool bDone = false;

float FOV = 0.f;
float ScreenAspectRatio = 0.f;
float fRenderLoadProcess = 0.0f;

DWORD GlobalWidth = 0;
DWORD GlobalHeight = 0;

IDXGISwapChain*                 pSwapChain          = nullptr;
ID3D11Device*                   pDevice             = nullptr;
ID3D11DeviceContext*            pContext            = nullptr;
ID3D11RenderTargetView*         pRTView             = nullptr;
ID3D11InputLayout*              pInputLayout        = nullptr;
ID3D11Texture2D*                pDepthStencilBuffer = nullptr;
ID3D11DepthStencilState*        pDepthStencilState  = nullptr;
ID3D11DepthStencilView*         pDepthStencilView   = nullptr;
ID3D11RasterizerState*          pRasterState        = nullptr;
ID3D11VertexShader*             pVertexShader       = nullptr;
ID3D11PixelShader*              pPixelShader        = nullptr;


ID3D11Buffer*                   pComputeConstBuffer = nullptr;
ID3D11Buffer*                   pComputeInputBuffer = nullptr;
ID3D11ComputeShader*            pComputeShader      = nullptr;
ID3D11Texture2D*                pComputeTexture     = nullptr;
ID3D11UnorderedAccessView*      pUAV                = nullptr;
ID3D11ShaderResourceView*       pSRV                = nullptr;

PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN pD3D11CreateDeviceAndSwapChain = nullptr;

LPCWSTR g_ShaderFile = L"Test.fx";

bool InitTexture(float width, float height);

bool CompileShaderFromFile(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR shaderModel, _Outptr_ ID3DBlob** blob)
{
    if (!srcFile || !entryPoint || !shaderModel || !blob)
        return false;

    *blob = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    size_t SrcDataSize = 0;
    void *data = 0;
    const bool loaded = LoadFile(srcFile, &data,&SrcDataSize);

	if (!loaded)
	{
		MessageBoxW(nullptr, L"Can't load shader file", L"≈√√Œ√", MB_OK | MB_ICONERROR);
		return false;
	}

    const HRESULT hr = D3DCompile(data, SrcDataSize, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint, shaderModel, flags, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        HeapFree(GetProcessHeap(), 0, data);
        if (errorBlob)
        {
            OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        }

        if (errorBlob)
            _RELEASE(errorBlob);

        return false;
    }

    *blob = shaderBlob;
    HeapFree(GetProcessHeap(), 0, data);
    return true;
}

bool InitVertexShader()
{
    ID3DBlob* vsBlob = nullptr;
    const bool hr = CompileShaderFromFile(g_ShaderFile, "VS", "vs_5_0", &vsBlob);

	if (!hr)
	{
		return false;
	}

    if (FAILED(pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVertexShader)))
    {
		MessageBoxW(nullptr, L"Can't compile vertex shader", L"≈√√Œ√", MB_OK | MB_ICONERROR);
        _RELEASE(vsBlob);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    const UINT numElements = ARRAYSIZE(layout);
    const HRESULT layoutResult = pDevice->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pInputLayout);
    _RELEASE(vsBlob);
    if (FAILED(layoutResult)) return false;

    pContext->IASetInputLayout(pInputLayout);

    return true;
}

bool InitPixelShader()
{
    ID3DBlob *psBlob = nullptr;
    const bool hr = CompileShaderFromFile(g_ShaderFile, "PS", "ps_5_0", &psBlob);
    if (!hr) return false;

    const HRESULT pixelResult = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPixelShader);
    _RELEASE(psBlob);

    return FAILED(pixelResult) ? false : true;
}

void SetProjectionMatrix(float width, float height)
{
	D3D11_VIEWPORT viewport;
	// Setup the viewport for rendering.
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

    InitTexture(width, height);
	// Create the viewport.
    pContext->RSSetViewports(1, &viewport);
}

bool InitTexture(float width, float height)
{
    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.ArraySize = 1;
    texDesc.Width = static_cast<unsigned>(width);
    texDesc.Height = static_cast<unsigned>(height);
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

	_RELEASE(pComputeTexture);
    auto hr = pDevice->CreateTexture2D(&texDesc, nullptr, &pComputeTexture);

    if(FAILED(hr)) return false;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
	_RELEASE(pUAV);
    hr = pDevice->CreateUnorderedAccessView(pComputeTexture, &uavDesc, &pUAV);
    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

	_RELEASE(pSRV);
    hr = pDevice->CreateShaderResourceView(pComputeTexture, &srvDesc, &pSRV);

    if(FAILED(hr)) return false;

    return true;
}

bool InitComputeShader(D3D11_VIEWPORT *view_port)
{
    ID3DBlob *csBlob = nullptr;

    HRESULT hr = CompileShaderFromFile(L"TestCompute.hlsl", "CSMain", "cs_5_0", &csBlob);

    if(FAILED(hr))
    {
        return false;
    }

    hr = pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &pComputeShader);

    _RELEASE(csBlob);

    if(FAILED(hr))
    {
        return false;
    }

    ComputeConstantBuffer constBuffer;

    constBuffer.resolution = DirectX::XMFLOAT2(view_port->Width, view_port->Height);
    constBuffer.time = 0.0f;

    D3D11_BUFFER_DESC constDesc;
    ZeroMemory(&constDesc, sizeof(constDesc));
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    constDesc.ByteWidth = sizeof(ComputeConstantBuffer);
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = &constBuffer;
    hr = pDevice->CreateBuffer(&constDesc, &data, &pComputeConstBuffer);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool InitGeometry(D3D11_VIEWPORT *view_port)
{
    SetProjectionMatrix(view_port->Width, view_port->Height);

    return true;
}

bool InitRender(bool IsImgui)
{
	int Times = 0;
	D3D_FEATURE_LEVEL feature_level;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ID3D11Texture2D* back_buffer = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;
	HRESULT hr = 0;

	if (!pD3D11CreateDeviceAndSwapChain)
	{

		HMODULE hLib = LoadLibraryA("d3d11.dll");
		pD3D11CreateDeviceAndSwapChain = reinterpret_cast<PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN>(GetProcAddress(hLib, "D3D11CreateDeviceAndSwapChain"));

		if (!pD3D11CreateDeviceAndSwapChain) return false;
	}

	void* hWnd = GetMainWindowHandle();
	if (!hWnd) return false;

	/*
		Init D3D11 device
	*/
	memset(&swap_chain_desc, 0, sizeof(swap_chain_desc));
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = static_cast<HWND>(hWnd);
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

#ifdef DEBUG
	UINT FlagDebug = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT FlagDebug = 0;
#endif

	if (FAILED(pD3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, FlagDebug, nullptr, 0, D3D11_SDK_VERSION, &swap_chain_desc, &pSwapChain, &pDevice, &feature_level, &pContext)))
	{
		/*
			If hardware device fails, then try WARP high-performance
			software rasterizer, this is useful for RDP sessions
		*/
		hr = pD3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swap_chain_desc, &pSwapChain, &pDevice, &feature_level, &pContext);
		if (FAILED(hr)) return false;
	}

	_RELEASE(pRTView);
    pContext->OMSetRenderTargets(0, nullptr, nullptr);

ResizeBuffers:
	/*
		Wait a lot of time, because this fail can be raised by
		user restarted video driver
	*/
    hr = pSwapChain->ResizeBuffers(0, BASE_WIDTH, BASE_HEIGHT, DXGI_FORMAT_UNKNOWN, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Sleep(500);
		goto ResizeBuffers;
	}
    if (hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
    {
        Sleep(500);
        Times++;

        if (Times >= 3)
        {
            return false;
        }

        goto ResizeBuffers;
    }
    if (FAILED(hr))
    {
        return false;
    }

    memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = pSwapChain->GetBuffer(0, A_IID_ID3D11Texture2D, reinterpret_cast<void **>(&back_buffer));
    hr = pDevice->CreateRenderTargetView(back_buffer, &desc, &pRTView);

    back_buffer->Release();

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = BASE_WIDTH;
	depthBufferDesc.Height = BASE_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	if (FAILED(pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthStencilBuffer)))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
    
	if (FAILED(pDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState)))
	{
		return false;
	}
    pContext->OMSetDepthStencilState(pDepthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
// 	if (FAILED(ID3D11Device_CreateDepthStencilView(pDevice, pDepthStencilBuffer, &depthStencilViewDesc, &pDepthStencilView)))
// 	{
// 		return false;
// 	}


	// Bind the render target view and depth stencil buffer to the output render pipeline.
    pContext->OMSetRenderTargets(1, &pRTView, pDepthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	if (FAILED(pDevice->CreateRasterizerState(&rasterDesc, &pRasterState)))
	{
		return false;
	}

	// Now set the rasterizer state.
    pContext->RSSetState(pRasterState);

	// Setup the viewport for rendering.
	viewport.Width = static_cast<float>(BASE_WIDTH);
	viewport.Height = static_cast<float>(BASE_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

    GlobalWidth = viewport.Width;
    GlobalHeight = viewport.Height;

	// Create the viewport.
    pContext->RSSetViewports(1, &viewport);

    const bool vertexShader = InitVertexShader();

    if (!vertexShader) return false;

    const bool pixelShader = InitPixelShader();
    if (!pixelShader) return false;

    const bool geometry = InitGeometry(&viewport);
    if (!geometry) return false;

    bool compute = InitComputeShader(&viewport);

    return compute;
}

void 
DestroyRender()
{
	if (pContext) pContext->ClearState();

	_RELEASE(pRTView);
	_RELEASE(pContext);
	_RELEASE(pDevice);
	_RELEASE(pSwapChain);
    _RELEASE(pVertexShader);
    _RELEASE(pPixelShader);
}

LPDWORD
GetMainWidth()
{
	return &GlobalWidth;
}


LPDWORD
GetMainHeight()
{
	return &GlobalHeight;
}

bool ResizeRender(int Width, int Height)
{
	if (!Width || !Height || !pContext) return false;

	ID3D11Texture2D* back_buffer = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC desc;

	pContext->OMSetRenderTargets(0, nullptr, nullptr);
    if (pRTView) pRTView->Release();

ResizeBuffers:
    const HRESULT hr = pSwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Sleep(500);
		goto ResizeBuffers;
	}
    if (hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
    {
        return false;
    }

    memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    pSwapChain->GetBuffer(0, A_IID_ID3D11Texture2D, reinterpret_cast<void **>(&back_buffer));
    pDevice->CreateRenderTargetView(back_buffer, &desc, &pRTView);

    back_buffer->Release();
    SetProjectionMatrix(static_cast<float>(Width), static_cast<float>(Height));

	GlobalWidth = Width;
	GlobalHeight = Height;

	return true;
}

void BindShaderMatrix()
{
    pContext->CSSetShader(pComputeShader, nullptr, 0);
    pContext->CSSetConstantBuffers(0, 1, &pComputeConstBuffer);
    pContext->CSSetConstantBuffers(1, 1, &pComputeInputBuffer);
    pContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);
    
    const int threadGroupX = (int)GlobalWidth;
    const int threadGroupY = (int)GlobalHeight;
    pContext->Dispatch(threadGroupX, threadGroupY, 1);
    ID3D11UnorderedAccessView* UAVHandlesnull = nullptr;
    pContext->CSSetUnorderedAccessViews(0, 1, &UAVHandlesnull, nullptr);
}

float orbit = 0.0f;

void RenderCube()
{
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->IASetInputLayout(nullptr);
    pContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

    ComputeConstantBuffer constBuffer;
    constBuffer.resolution = DirectX::XMFLOAT2(float(GlobalWidth), float(GlobalHeight));
    const auto timer = GetTickCount64();

    constBuffer.time = static_cast<float>(timer) * 0.005;

    pContext->UpdateSubresource(pComputeConstBuffer, 0, nullptr, &constBuffer, 0, 0);

    pContext->VSSetShader(pVertexShader, nullptr, 0);
    pContext->PSSetShader(pPixelShader, nullptr, 0);
    pContext->PSSetShaderResources(0, 1, &pSRV);

    pContext->Draw(3, 0);
}

bool RenderDraw()
{
	static bool isFirst = true;
	HRESULT hr;

#ifdef DEBUG
	static LARGE_INTEGER Freq;
	static LARGE_INTEGER FpsToShow;
	static size_t AllFramesCount = 0;

	if (isFirst)
	{
		QueryPerformanceFrequency(&Freq);
		QueryPerformanceCounter(&FpsToShow);

		isFirst = false;
	}
#endif
	
    /* Draw */
    pContext->ClearRenderTargetView(pRTView, BackGroundTest);
    //ID3D11DeviceContext_ClearDepthStencilView(pContext, pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
    pContext->OMSetRenderTargets(1, &pRTView, nullptr);

    BindShaderMatrix();
    RenderCube();
    hr = pSwapChain->Present(bVSync, 0);
    
	/*
		If we lost device (such as in D3D9), we need to wait it
	*/
	if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		return false;
	}
    if (hr == DXGI_STATUS_OCCLUDED)
    {
        /*
			Window is not visible, so VSync won't work. Let's sleep a bit to reduce CPU usage
		*/
        Sleep(10);
    }
    else if (FAILED(hr))
    {
        return false;
    }

#ifdef DEBUG
	LARGE_INTEGER TempTime;
	QueryPerformanceCounter(&TempTime);
	AllFramesCount++;

	const float DeltaTime = static_cast<float>(TempTime.QuadPart - FpsToShow.QuadPart) / static_cast<float>(Freq.QuadPart);
	if (DeltaTime > 0.2f)
	{
		FpsToShow.QuadPart = TempTime.QuadPart;

		char WindowText[64];
		memset(WindowText, 0, sizeof(WindowText));

		sprintf_s(WindowText, 64, "Alistra -%4.0f FPS", AllFramesCount / DeltaTime);
		SetWindowTextA(GetMainWindowHandle(), WindowText);
		AllFramesCount = 0;
	}
#endif

	return true;
}

float GetRenderLoadProcess()
{
	return fRenderLoadProcess;
}

bool IsRenderWorkDone()
{
	return true;
}