#include "stdafx.h"
#include "DebugCube.h"

DebugCube::DebugCube(D3DXVECTOR3 center, float width, float height, float depth)
	:DebugObj(8, 24), centerPos(center), width(width), height(height), depth(depth)
{
	//머터리얼
	material = new Material(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer;
	colorBuffer = new ColorBuffer;

	D3DXMatrixIdentity(&cubeWorld);
	// setting vertices
	{
		vertices = new VertexColor[vertexCount];

		vertices[0].Position = D3DXVECTOR3(center.x - width, center.y - depth, center.z - height);
		vertices[1].Position = D3DXVECTOR3(center.x - width, center.y + depth, center.z - height);
		vertices[2].Position = D3DXVECTOR3(center.x + width, center.y - depth, center.z - height);
		vertices[3].Position = D3DXVECTOR3(center.x + width, center.y + depth, center.z - height);

		vertices[4].Position = D3DXVECTOR3(center.x - width, center.y - depth, center.z + height);
		vertices[5].Position = D3DXVECTOR3(center.x - width, center.y + depth, center.z + height);
		vertices[6].Position = D3DXVECTOR3(center.x + width, center.y - depth, center.z + height);
		vertices[7].Position = D3DXVECTOR3(center.x + width, center.y + depth, center.z + height);
	}

	// setting indexCount
	{
		indices = new UINT[24]
		{
			0,1,0,2,1,3,3,2,
			4,5,4,6,5,7,7,6,
			0,4,1,5,2,6,3,7
		};
	}

	//create vertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc,
			&data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data,
			&indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

DebugCube::~DebugCube()
{
}

void DebugCube::ChangeColor(D3DXCOLOR color)
{
	colorBuffer->Data.color = color;
}

void DebugCube::Update(D3DXMATRIX parentMatrix)
{
	cubeWorld = parentMatrix;
}

void DebugCube::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(10);
	material->PSSetBuffer();
	
	worldBuffer->SetMatrix(cubeWorld);
	worldBuffer->SetVSBuffer(1);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}
