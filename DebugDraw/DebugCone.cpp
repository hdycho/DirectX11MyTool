#include "stdafx.h"
#include "DebugCone.h"

DebugCone::DebugCone(float angle, D3DXVECTOR3 dir, D3DXVECTOR3 pos)
	:DebugObj(1,1)
{
	material = new Material(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();

	//버텍스 데이터
	{
		vertexCount = 21; // 육각뿔모양으로 만들것
		vertices = new VertexColor[vertexCount];
		
		float height = 100;
		float radius = height*tanf(angle);
		float xzAngle = Math::PI / ((vertexCount-1)/2);

		D3DXVECTOR3 statrPos = pos;
		
		vertices[0].Position = pos;
		vertices[0].Color = D3DXCOLOR(1, 1, 1, 1);


		float deltaXZangle = 0;
		for (int i = 1; i < vertexCount; i++)
		{
			vertices[i].Position.x = vertices[0].Position.x + radius*cosf(deltaXZangle) + 2 * height*dir.x;
			vertices[i].Position.y = vertices[0].Position.y - height + 2 * height*dir.y;
			vertices[i].Position.z = vertices[0].Position.z + radius*sinf(deltaXZangle) + 2 * height*dir.z;
			
			deltaXZangle += xzAngle;
		}
	}

	//인덱스 데이터
	{
		indexCount = (vertexCount-1)*2;
		indices = new UINT[indexCount];

		int idx = 0;
		for (int j = 0; j < indexCount/2; j++)
		{
			indices[j * 2 + 0] = 0;
			indices[j * 2 + 1] = j + 1;
			idx += 2;
		}
		UINT index = 0;	
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

DebugCone::~DebugCone()
{
}

void DebugCone::ChangeConeShape(float angle, D3DXVECTOR3 Dir, D3DXVECTOR3 pos)
{
	//버텍스 데이터
	{
		vertexCount = 21; // 육각뿔모양으로 만들것
		vertices = new VertexColor[vertexCount];

		float height = 100;
		float radius = height*tanf(angle);
		float xzAngle = Math::PI / ((vertexCount - 1) / 2);

		D3DXVECTOR3 statrPos = pos;

		vertices[0].Position = pos;
		vertices[0].Color = D3DXCOLOR(1, 1, 1, 1);

		float deltaXZangle = 0;
		for (int i = 1; i < vertexCount; i++)
		{
			vertices[i].Position.x = vertices[0].Position.x + radius*cosf(deltaXZangle)+2*height*Dir.x;
			vertices[i].Position.y = vertices[0].Position.y - height+ 2*height*Dir.y;
			vertices[i].Position.z = vertices[0].Position.z + radius*sinf(deltaXZangle)+ 2*height*Dir.z;

			deltaXZangle += xzAngle;
		}


	}
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormal), vertexCount);
}

void DebugCone::ChangeColor(D3DXCOLOR color)
{
	colorBuffer->Data.color = color;
}

void DebugCone::Update(D3DXMATRIX parentMatrix)
{
}

void DebugCone::Render()
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
