#include "stdafx.h"
#include "DebugRect.h"

DebugRect::DebugRect(RECTTYPE type,float radius, D3DXVECTOR3 pos, D3DXVECTOR3 sacle)
	:DebugObj(1, 1)
{
	rtype = type;
	material = new Material(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();

	//버텍스데이터
	{
		vertexCount = 4;
		vertices = new VertexColor[vertexCount];

		float width, height;
		width = height = radius;
		
		if (rtype == FRONT)
		{
			vertices[0].Position = D3DXVECTOR3(pos.x - width, pos.y - height, pos.z);
			vertices[1].Position = D3DXVECTOR3(pos.x + width, pos.y - height, pos.z);
			vertices[2].Position = D3DXVECTOR3(pos.x + width, pos.y + height, pos.z);
			vertices[3].Position = D3DXVECTOR3(pos.x - width, pos.y + height, pos.z);
		}
		else if (rtype == UP)
		{
			vertices[0].Position = D3DXVECTOR3(pos.x - width, pos.y, pos.z - height);
			vertices[1].Position = D3DXVECTOR3(pos.x + width, pos.y, pos.z - height);
			vertices[2].Position = D3DXVECTOR3(pos.x + width, pos.y, pos.z + height);
			vertices[3].Position = D3DXVECTOR3(pos.x - width, pos.y, pos.z + height);
		}
	}

	//인덱스데이터
	{
		indexCount = 8;
		indices = new UINT[indexCount]
		{
			0,1,
			1,2,
			2,3,
			3,0
		};
	}

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

DebugRect::DebugRect(RECTTYPE type, D3DXVECTOR3 * pos)
	:DebugObj(1, 1)
{
	rtype = type;
	material = new Material(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();

	//버텍스데이터
	{
		vertexCount = 4;
		vertices = new VertexColor[vertexCount];

		if (rtype == POINT)
		{
			vertices[0].Position = D3DXVECTOR3(pos[0].x, pos[0].y, pos[0].z);
			vertices[1].Position = D3DXVECTOR3(pos[1].x, pos[1].y, pos[1].z);
			vertices[2].Position = D3DXVECTOR3(pos[2].x, pos[2].y, pos[2].z);
			vertices[3].Position = D3DXVECTOR3(pos[3].x, pos[3].y, pos[3].z);
		}
	}

	//인덱스데이터
	{
		indexCount = 8;
		indices = new UINT[indexCount]
		{
			0,1,
			1,2,
			2,3,
			3,0
		};
	}

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

DebugRect::~DebugRect()
{
}


void DebugRect::ChangeRectPoint(D3DXVECTOR3 * pos)
{
	//버텍스데이터
	{
		vertexCount = 4;
		vertices = new VertexColor[vertexCount];

		if (rtype == POINT)
		{
			vertices[0].Position = D3DXVECTOR3(pos[0].x, pos[0].y, pos[0].z);
			vertices[1].Position = D3DXVECTOR3(pos[1].x, pos[1].y, pos[1].z);
			vertices[2].Position = D3DXVECTOR3(pos[2].x, pos[2].y, pos[2].z);
			vertices[3].Position = D3DXVECTOR3(pos[3].x, pos[3].y, pos[3].z);
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormal), vertexCount);
}

void DebugRect::ChangeColor(D3DXCOLOR color)
{
	colorBuffer->Data.color = color;
}

void DebugRect::Update(D3DXMATRIX parentMatrix)
{
}

void DebugRect::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(10);
	material->PSSetBuffer();
	worldBuffer->SetVSBuffer(1);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}
