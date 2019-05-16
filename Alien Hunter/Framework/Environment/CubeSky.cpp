#include "Framework.h"
#include "CubeSky.h"
#include "../Viewer/Camera.h"

CubeSky::CubeSky(wstring cubeFile)
{
	srv = NULL;
	isPick = false;
	material = new Material(Effects + L"008_CubeSky.hlsl");
	sphere = new MeshSphere(material, 500.0f, 30, 30);

	fileName = cubeFile;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(), fileName.c_str(), NULL, NULL, &srv, NULL);
	assert(SUCCEEDED(hr));

	//ID3D11Texture2D* texture;
	//srv->GetResource((ID3D11Resource **)&texture);

	//D3D11_TEXTURE2D_DESC textureDesc;
	//texture->GetDesc(&textureDesc);

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srv->GetDesc(&srvDesc);

	srvVariable = material->GetEffect()->AsSRV("CubeMap");
	srvVariable->SetResource(srv);
}

CubeSky::~CubeSky()
{
	SAFE_DELETE(sphere);
	SAFE_DELETE(material);

	SAFE_RELEASE(srv);
}

void CubeSky::Update()
{
	D3DXVECTOR3 position;
	Context::Get()->GetMainCamera()->Position(&position);

	sphere->Position(position);
}

void CubeSky::PreRender()
{
}

void CubeSky::Render()
{
	sphere->Render();
}

void CubeSky::CreateSRV(wstring fileName)
{
	this->fileName = fileName;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(), this->fileName.c_str(), NULL, NULL, &srv, NULL);
	assert(SUCCEEDED(hr));
}
