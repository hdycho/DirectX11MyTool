#include "Framework.h"
#include "D3D.h"

D3D* D3D::instance = NULL;

D3DDesc D3D::d3dDesc;
ID3D11Device* D3D::device = NULL;
ID3D11DeviceContext* D3D::deviceContext = NULL;
IDXGISwapChain* D3D::swapChain = NULL;

D3D * D3D::Get()
{
	//assert(instance != NULL);

	return instance;
}

void D3D::Create()
{
	assert(instance == NULL);

	instance = new D3D();
}

void D3D::Delete()
{
	SAFE_DELETE(instance);
}

void D3D::SetRenderTarget(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv)
{
	if (rtv == NULL)
		rtv = renderTargetView;

	if (dsv == NULL)
		dsv = depthStencilView;

	D3D::GetDC()->OMSetRenderTargets(1, &rtv, dsv);
}

void D3D::SetRenderTargets(UINT count, ID3D11RenderTargetView ** rtvs, ID3D11DepthStencilView * dsv)
{
	D3D::GetDC()->OMSetRenderTargets(count, rtvs, dsv);
}

void D3D::Clear(D3DXCOLOR color, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	if (rtv == NULL)
		rtv = renderTargetView;

	if (dsv == NULL)
		dsv = depthStencilView;

	deviceContext->ClearRenderTargetView(rtv, color);
	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1, 0);
}

void D3D::Present()
{
	swapChain->Present(d3dDesc.bVsync == true ? 1 : 0, 0);
}

void D3D::ResizeScreen(float width, float height)
{
	if (width < 1 || height < 1)
		return;

	d3dDesc.Width = width;
	d3dDesc.Height = height;
		
	DeleteBackBuffer();
	{
		HRESULT hr = swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(hr));
	}
	CreateBackBuffer(width, height);
}

D3D::D3D()
	: numerator(0), denominator(1)
{
	SetGpuInfo(); // gpu 정보 설정하는 함수
	// 9에서는 gpu cpu 혼합 렌더링했음 과도기적이어서
	// 11부터는 무조건 렌더링은 gpu 그래서 시작부터 gpu 세팅

	CreateSwapChainAndDevice();
	CreateBackBuffer(d3dDesc.Width, d3dDesc.Height);
}

D3D::~D3D()
{
	DeleteBackBuffer();

	if (swapChain != NULL)
		swapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(device);
	SAFE_RELEASE(swapChain);
}

void D3D::SetGpuInfo()
{
	HRESULT hr;

	IDXGIFactory* factory;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
	assert(SUCCEEDED(hr));

	// 해외에선 그래픽 어답터라고 함 그래픽 카드를

	IDXGIAdapter* adapter;
	hr = factory->EnumAdapters(0, &adapter);
	assert(SUCCEEDED(hr));

	// gpu 정보 가져오는거
	IDXGIOutput* adapterOutput;
	hr = adapter->EnumOutputs(0, &adapterOutput);
	assert(SUCCEEDED(hr));

	UINT modeCount;
	hr = adapterOutput->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM
		, DXGI_ENUM_MODES_INTERLACED
		, &modeCount
		, NULL
	);
	assert(SUCCEEDED(hr));

	// r8g8b8a8
	// 0 ~ 255(0xff) 앞으로 16진수 많이 다룰꺼

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[modeCount];
	hr = adapterOutput->GetDisplayModeList
	(
		// 16진수 형의 unsinged 쓰겠다는거 (UNORM)
		DXGI_FORMAT_R8G8B8A8_UNORM
		, DXGI_ENUM_MODES_INTERLACED
		, &modeCount
		, displayModeList
	);
	assert(SUCCEEDED(hr));

	for (UINT i = 0; i < modeCount; i++)
	{
		bool isCheck = true;
		isCheck &= displayModeList[i].Width == d3dDesc.Width;
		isCheck &= displayModeList[i].Height == d3dDesc.Height;

		if (isCheck == true)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	hr = adapter->GetDesc(&adapterDesc);
	assert(SUCCEEDED(hr));

	gpuMemorySize = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	gpuDescription = adapterDesc.Description;
	
	SAFE_DELETE_ARRAY(displayModeList);

	
	SAFE_RELEASE(adapterOutput);
	SAFE_RELEASE(adapter);
	SAFE_RELEASE(factory);
}

// swapchain 
// 렌더링될 창 하나당 swapchain이 붙음 렌더링 단위라고 보면됨
// 9때는 swapchain 만드는게 복잡했는데
// 11에는 많이 편해짐
// 수업 중에 쓸일은 없음 max 같은데서 씀 (창이 여러개 있어야 할때)

void D3D::CreateSwapChainAndDevice()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (d3dDesc.bVsync == true)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = d3dDesc.Handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !d3dDesc.bFullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 지원할 수 있는 dx 버전이라는 의미
	// 가끔 그래픽 카드가 지원하지 않는 경우가 있음
	// 그 경우 11_1 주석 해버림 됨 (11_1이 11.1 버전이란거)
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL
		, D3D_DRIVER_TYPE_HARDWARE
		, NULL
		, creationFlags
		, featureLevels
		, 1
		, D3D11_SDK_VERSION
		, &swapChainDesc
		, &swapChain
		, &device
		, NULL
		, &deviceContext
	);
	assert(SUCCEEDED(hr));
}

void D3D::CreateBackBuffer(float width, float height)
{
	HRESULT hr;

	// backbuffer(사실 texture라고 보면됨) 나오면 
	// depthstencil이 나와야 실제 backbuffer가 만들어짐
	// Create BackBuffer
	{
		ID3D11Texture2D* backbufferPointer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbufferPointer);
		assert(SUCCEEDED(hr));

		// 시스템에 기본적으로 붙어있는 렌더 타겟 뷰
		hr = D3D::GetDevice()->CreateRenderTargetView(backbufferPointer, NULL, &renderTargetView);
		assert(SUCCEEDED(hr));

		SAFE_RELEASE(backbufferPointer);
	}
	
	// 텍스처 생성 9이랑 많이 다름
	// Create Texture - DSV 용
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &backBuffer);
		assert(SUCCEEDED(hr));
	}

	// Create DSV (Depth Stencil View)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		hr = D3D::GetDevice()->CreateDepthStencilView(backBuffer, &desc, &depthStencilView);
		assert(SUCCEEDED(hr));

		SetRenderTarget(renderTargetView, depthStencilView);
	}
}

void D3D::DeleteBackBuffer()
{
	SAFE_RELEASE(depthStencilView);
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(backBuffer);
}