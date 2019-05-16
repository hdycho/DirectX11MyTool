#include "Framework.h"
#include "DirectWrite.h"

DirectWrite* DirectWrite::instance = NULL;
ID2D1DeviceContext* DirectWrite::deviceContext = NULL;
IDWriteFactory* DirectWrite::writeFactory = NULL;
ID2D1Bitmap1* DirectWrite::targetBitmap = NULL;
IDXGISurface* DirectWrite::dxgiSurface = NULL;

vector<FontBrushDesc> DirectWrite::fontBrush;
vector<FontTextDesc> DirectWrite::fontText;

void DirectWrite::Create()
{
	assert(instance == NULL);

	instance = new DirectWrite();
}

void DirectWrite::Delete()
{
	assert(instance != NULL);

	SAFE_DELETE(instance);
}

DirectWrite * DirectWrite::Get()
{
	return instance;
}

void DirectWrite::CreateSurface()
{
	HRESULT hr = D3D::GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void **)&dxgiSurface);
	assert(SUCCEEDED(hr));

	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE; // 알파 안쓸꺼
													   // dot per inch 실제 윈도우 크기가 다르더라도 맞추려고 쓰는거
	bp.dpiX = 96;
	bp.dpiY = 96;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = NULL;

	hr = deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface, &bp, &targetBitmap);
	assert(SUCCEEDED(hr));

	deviceContext->SetTarget(targetBitmap);
}

void DirectWrite::DeleteSurface()
{
	deviceContext->SetTarget(NULL);

	for (FontBrushDesc desc : fontBrush)
		SAFE_RELEASE(desc.Brush);
	fontBrush.clear();

	for (FontTextDesc desc : fontText)
		SAFE_RELEASE(desc.Format);
	fontText.clear();

	SAFE_RELEASE(targetBitmap);
	SAFE_RELEASE(dxgiSurface);
}

DirectWrite::DirectWrite()
{
	HRESULT hr;

	// shared 쓰는 이유 3D랑 동일 시키려고 쓰는거
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&writeFactory);
	assert(SUCCEEDED(hr));

	D2D1_FACTORY_OPTIONS option;
	option.debugLevel = D2D1_DEBUG_LEVEL_NONE;
	// 2D때는 싱글 스레드로해도 문제없는데 3D면 싱크 안맞으면 터져버림
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	assert(SUCCEEDED(hr));

	// DirectX Graphic Interface - GPU 
	IDXGIDevice* dxgiDevice;
	hr = D3D::GetDevice()->QueryInterface(&dxgiDevice);
	assert(SUCCEEDED(hr));
	
	hr = factory->CreateDevice(dxgiDevice, &device);
	assert(SUCCEEDED(hr));
	dxgiDevice->Release();

	hr = device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&deviceContext);
	assert(SUCCEEDED(hr));
	
	CreateSurface();

	////TODO : 테스트용
	//hr = deviceContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &brush);
	//assert(SUCCEEDED(hr));
	//hr = writeFactory->CreateTextFormat(L"궁서", NULL,
	//	DWRITE_FONT_WEIGHT_NORMAL, // 두께
	//	DWRITE_FONT_STYLE_NORMAL, // 스타일
	//	DWRITE_FONT_STRETCH_NORMAL, // 보정
	//	40.0f, // 글자 크기
	//	L"ko", &textFormat// 영어 en-us, ko-kr en-kr 이런거 쓰면됨
	//	);
	//assert(SUCCEEDED(hr));
}

DirectWrite::~DirectWrite()
{
	DeleteSurface();

	SAFE_RELEASE(factory);
	SAFE_RELEASE(writeFactory);

	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(device);
}

void DirectWrite::RenderText(wstring text, RECT rect, float size, wstring font, D3DXCOLOR color, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch)
{
	FontBrushDesc brushDesc;
	brushDesc.Color = color;

	FontBrushDesc* findBrush = NULL;
	for (FontBrushDesc& desc : fontBrush)
	{
		if (desc == brushDesc)
		{
			findBrush = &desc;
			break;
		}
	}

	if (findBrush == NULL)
	{
		D2D1::ColorF colorF = D2D1::ColorF(color.r, color.g, color.b);
		deviceContext->CreateSolidColorBrush(colorF, &brushDesc.Brush);

		fontBrush.push_back(brushDesc);
		findBrush = &brushDesc;
	}

	FontTextDesc textDesc;
	textDesc.Font = font;
	textDesc.FontSize = size;
	textDesc.Stretch = stretch;
	textDesc.Style = style;
	textDesc.Weight = weight;

	FontTextDesc* findText = NULL;
	for (FontTextDesc& desc : fontText)
	{
		if (desc == textDesc)
		{
			findText = &desc;
			break;
		}
	}

	if (findText == NULL)
	{
		writeFactory->CreateTextFormat(
			textDesc.Font.c_str(), NULL, textDesc.Weight, textDesc.Style, textDesc.Stretch,
			textDesc.FontSize, L"ko", &textDesc.Format
		);

		fontText.push_back(textDesc);
		findText = &textDesc;
	}

	D2D1_RECT_F rectF;
	rectF.left = (float)rect.left;
	rectF.top = (float)rect.top;
	rectF.right = (float)rect.right;
	rectF.bottom = (float)rect.bottom;

	deviceContext->DrawTextW(
		text.c_str(), text.size(), findText->Format, rectF, findBrush->Brush
	);
}

//void DirectWrite::RenderText(wstring text)
//{
//	D2D1_RECT_F rect;
//	rect.left = 100;
//	rect.right = 500;
//	rect.top = 0;
//	rect.bottom = 1000;
//
//	// 마지막 옵션은 UI 크기 달라질때 그려질때 씀
//	deviceContext->DrawTextW(text.c_str(), text.length(), textFormat,
//		rect, // 출력될 위치
//		brush);
//}
