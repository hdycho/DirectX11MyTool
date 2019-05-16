#include "stdafx.h"
#include "TestTexture.h"
#include "Fbx/FbxLoader.h"

void TestTexture::Initialize()
{

}

void TestTexture::Ready()
{
	vertices[0].Position = D3DXVECTOR3(-0.2f, -0.2f, 0.0f);
	vertices[1].Position = D3DXVECTOR3(-0.2f, 0.2f, 0.0f);
	vertices[2].Position = D3DXVECTOR3(0.2f, -0.2f, 0.0f);
	vertices[3].Position = D3DXVECTOR3(0.2f, -0.2f, 0.0f);
	vertices[4].Position = D3DXVECTOR3(-0.2f, 0.2f, 0.0f);
	vertices[5].Position = D3DXVECTOR3(0.2f, 0.2f, 0.0f);
	CsResource::CreateRawBuffer(sizeof(Vertex) * 6, vertices, &vertexBuffer[0]);

	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	//   128개의 매트릭스를 만듬
	//   한 픽셀당 16바이트 이기 때문에 행열은 64바이트 이기 때문에 4를 곱해줌
	destDesc.Width = 128 * 4;
	destDesc.Height = 128;
	destDesc.MipLevels = 1;
	destDesc.ArraySize = 1;
	destDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	destDesc.SampleDesc.Count = 1;
	destDesc.SampleDesc.Quality = 0;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//destDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	destDesc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &worldData);
	assert(SUCCEEDED(hr));

	CsResource::CreateSRV(worldData, &worldDataSRV);

	D3DXMatrixTranslation(&world[0][0], 0.5f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&world[0][1], 0.0f, 0.5f, 0.0f);
	D3DXMatrixTranslation(&world[0][2], 0.0f, 0.0f, 0.1f);

	D3DXMatrixTranslation(&world[1][0], -0.5f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&world[1][1], 0.0f, -0.5f, 0.0f);
	D3DXMatrixTranslation(&world[1][2], 0.0f, 0.0f, 0.1f);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(worldData, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		for (int i = 0; i < 2; i++)
		{
			void * p = ((D3DXMATRIX *)subResource.pData) + (i * (128));

			memcpy(p, world[i], sizeof(D3DXMATRIX) * 3);
		}
	}
	D3D::GetDC()->Unmap(worldData, 0);

	colors[0] = D3DXCOLOR(1, 0, 0, 1);
	colors[1] = D3DXCOLOR(0, 1, 0, 1);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(D3DXCOLOR) * 2;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = colors;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	effect = new Effect(Effects + L"01_TestTexture.hlsl");
	effect->AsSRV("WorldData")->SetResource(worldDataSRV);
}

void TestTexture::Destroy()
{
	SAFE_DELETE(effect);
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	SAFE_RELEASE(worldData);
	SAFE_RELEASE(worldDataSRV);
}

void TestTexture::Update()
{
}

void TestTexture::PreRender()
{
}

void TestTexture::Render()
{
	UINT stride[] = { sizeof(Vertex), sizeof(D3DXCOLOR) };
	UINT offset[] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	effect->DrawInstanced(0, 0, 6, 2);
}

void TestTexture::PostRender()
{
}

void TestTexture::ResizeScreen()
{
}