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
	//����ڰ� ����� RTV�� �����
	ID3D11Texture2D*rtvTexture;
	//�ȼ��� ������ RTV
	ID3D11RenderTargetView*rtv;

	//����ڰ� ����� DSV�� ����
	ID3D11Texture2D*dsvTexture;
	//���̸� ������ dsv
	ID3D11DepthStencilView*dsv;

	class Viewport*viewport;
};