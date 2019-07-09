#include "Base.h"
#include "Base_Window.h"
#include "Base_Render.h"
#include <stdio.h>

#define COBJMACROS
#include <d3d11.h>

//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "d3d11.lib")

float BackGroundTest[] = { 0.1f, 0.1f, 0.1f, 1.0f };

DEFINE_IID(ID3D11Texture2D, 6f15aaf2, d208, 4e89, 9a, b4, 48, 95, 35, d3, 4f, 9c);

// it is never ImGui
boolean bImgui = false;
boolean bVSync = true;
boolean bDone = false;

float FOV = 0.f;
float ScreenAspectRatio = 0.f;
float fRenderLoadProcess = 0.0f;

DWORD GlobalWidth = 0;
DWORD GlobalHeight = 0;

IDXGISwapChain* pSwapChain = NULL;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* pRTView = NULL;
ID3D11InputLayout* pInputLayout = NULL;
ID3D11Texture2D* pDepthStencilBuffer;
ID3D11DepthStencilState* pDepthStencilState;
ID3D11DepthStencilView* pDepthStencilView;
ID3D11RasterizerState* pRasterState;
PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN pD3D11CreateDeviceAndSwapChain = NULL;

char 
InitRender(
	char IsImgui
)
{
	int Times = 0;
	D3D_FEATURE_LEVEL feature_level;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ID3D11Texture2D* back_buffer = NULL;
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
		pD3D11CreateDeviceAndSwapChain = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)GetProcAddress(hLib, "D3D11CreateDeviceAndSwapChain");

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
	swap_chain_desc.OutputWindow = hWnd;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

	if (FAILED(pD3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &swap_chain_desc, &pSwapChain, &pDevice, &feature_level, &pContext)))
	{
		/*
			If hardware device fails, then try WARP high-performance
			software rasterizer, this is useful for RDP sessions
		*/
		hr = pD3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &swap_chain_desc, &pSwapChain, &pDevice, &feature_level, &pContext);
		if (FAILED(hr)) return false;
	}

	_RELEASE(pRTView);

	ID3D11DeviceContext_OMSetRenderTargets(pContext, 0, NULL, NULL);

ResizeBuffers:
	/*
		Wait a lot of time, because this fail can be raised by
		user restarted video driver
	*/
	hr = IDXGISwapChain_ResizeBuffers(pSwapChain, 0, BASE_WIDTH, BASE_HEIGHT, DXGI_FORMAT_UNKNOWN, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Sleep(500);
		goto ResizeBuffers;
	}
	else if (hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
	{
		Sleep(500);
		Times++;

		if (Times >= 3)
		{
			return false;
		}

		goto ResizeBuffers;
	}
	else if (FAILED(hr))
	{
		return false;
	}

	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = IDXGISwapChain_GetBuffer(pSwapChain, 0, &A_IID_ID3D11Texture2D, (void **)&back_buffer);
	hr = ID3D11Device_CreateRenderTargetView(pDevice, (ID3D11Resource *)back_buffer, &desc, &pRTView);

	ID3D11Texture2D_Release(back_buffer);

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
	if (FAILED(ID3D11Device_CreateTexture2D(pDevice, &depthBufferDesc, NULL, &pDepthStencilBuffer)))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
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
	if (FAILED(ID3D11Device_CreateDepthStencilState(pDevice, &depthStencilDesc, &pDepthStencilState)))
	{
		return false;
	}

	ID3D11DeviceContext_OMSetDepthStencilState(pContext, pDepthStencilState, 1);

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
	ID3D11DeviceContext_OMSetRenderTargets(pContext, 1, &pRTView, pDepthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	if (FAILED(ID3D11Device_CreateRasterizerState(pDevice, &rasterDesc, &pRasterState)))
	{
		return false;
	}

	// Now set the rasterizer state.
	ID3D11DeviceContext_RSSetState(pContext, pRasterState);

	// Setup the viewport for rendering.
	viewport.Width = (float)BASE_WIDTH;
	viewport.Height = (float)BASE_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	ID3D11DeviceContext_RSSetViewports(pContext, 1, &viewport);

	return true;
}

void 
DestroyRender()
{
	if (pContext) ID3D11DeviceContext_ClearState(pContext);

	_RELEASE(pRTView);
	_RELEASE(pContext);
	_RELEASE(pDevice);
	_RELEASE(pSwapChain);
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

boolean
ResizeRender(
	int Width,
	int Height
)
{
	if (!Width || !Height || !pContext) return false;

	ID3D11Texture2D* back_buffer = NULL;
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	HRESULT hr = 0;

	if (pRTView) ID3D11RenderTargetView_Release(pRTView);

	ID3D11DeviceContext_OMSetRenderTargets(pContext, 0, NULL, NULL);

ResizeBuffers:
	hr = IDXGISwapChain_ResizeBuffers(pSwapChain, 0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Sleep(500);
		goto ResizeBuffers;
	}
	else if (hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
	{
		return false;
	}

	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = IDXGISwapChain_GetBuffer(pSwapChain, 0, &A_IID_ID3D11Texture2D, (void **)&back_buffer);
	hr = ID3D11Device_CreateRenderTargetView(pDevice, (ID3D11Resource *)back_buffer, &desc, &pRTView);

	ID3D11Texture2D_Release(back_buffer);

	GlobalWidth = Width;
	GlobalHeight = Height;

	return true;
}

boolean
RenderDraw()
{
	static boolean isFirst = true;
	HRESULT hr = 0;

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
	ID3D11DeviceContext_ClearRenderTargetView(pContext, pRTView, BackGroundTest);
	ID3D11DeviceContext_OMSetRenderTargets(pContext, 1, &pRTView, NULL);

	hr = IDXGISwapChain_Present(pSwapChain, bVSync, 0);

	/*
		If we lost device (such as in D3D9), we need to wait it
	*/
	if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		return false;
	}
	else if (hr == DXGI_STATUS_OCCLUDED)
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

	const float DeltaTime = (TempTime.QuadPart - FpsToShow.QuadPart) / (float)Freq.QuadPart;
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

float
GetRenderLoadProcess()
{
	return fRenderLoadProcess;
}

boolean
IsRenderWorkDone()
{
	return true;
}