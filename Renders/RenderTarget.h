#pragma once

class RenderTarget
{
public:
	RenderTarget(struct ExecuteValues*values,UINT width=0,UINT height=0);
	~RenderTarget();

	void Set(D3DXCOLOR clear = 0xFF000000);

	ID3D11ShaderResourceView*GetSRV() { return srv; }
	void SaveRTVTextrue(wstring saveFile);

	ID3D11RenderTargetView*GetRTV() { return rtv; }
	ID3D11DepthStencilView*GetDSV() { return dsv; }
private:
	struct ExecuteValues*values;

	UINT width, height;

	ID3D11ShaderResourceView*srv;
	//사용자가 사용할 RTV용 백버퍼
	ID3D11Texture2D*rtvTexture;
	//픽셀을 저장할 RTV
	ID3D11RenderTargetView*rtv;

	//사용자가 사용할 DSV용 버퍼
	ID3D11Texture2D*dsvTexture;
	//깊이를 저장할 dsv
	ID3D11DepthStencilView*dsv;

	class Viewport*viewport;
};