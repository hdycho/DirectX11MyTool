#include "Framework.h"
#include "RenderTargetView.h"

RenderTargetView::RenderTargetView(UINT width, UINT height, DXGI_FORMAT format)
	: width(width), height(height), format(format)
{
	// Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | 
			D3D11_BIND_UNORDERED_ACCESS;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
		assert(SUCCEEDED(hr));
	}

	// Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(texture, &desc, &rtv);
		assert(SUCCEEDED(hr));
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	// Create UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetView::~RenderTargetView()
{
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(srv);
	SAFE_RELEASE(uav);
	SAFE_RELEASE(texture);
}

void RenderTargetView::Clear(D3DXCOLOR clear)
{
	D3D::Get()->Clear(clear, rtv, D3D::Get()->GetDepthStencilView());
}
