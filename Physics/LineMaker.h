#pragma once
#include "stdafx.h"

class LineMaker
{
private:
	class LineColor :public ShaderBuffer
	{
	public:
		LineColor()
			:ShaderBuffer(&Data, sizeof(Data))
		{}
		struct Struct
		{
			D3DXCOLOR color;
		}Data;
	};
private:
	Shader*shader;
	float lineLengh;
	D3DXVECTOR3 lineDir;
	int vertexIdx;

	UINT vertexCount, indexCount;

	ID3D11Buffer*vertexBuffer;
	ID3D11Buffer*indexBuffer;

	VertexColor*vertices;
	UINT*indices;

	WorldBuffer*worldBuffer;
	LineColor* lineColor;
private:
	void SetVertexBuffer()
	{
		for (UINT i = 0; i < indexCount; i++)
			indices[i] = i;

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
public:
	LineMaker()
	{
		shader = new Shader(Shaders + L"003_Color.hlsl");
		worldBuffer = new WorldBuffer;
		lineColor = new LineColor();

		vertices = new VertexColor[2];
		indices = new UINT[2];
	}
	~LineMaker()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);

		SAFE_DELETE_ARRAY(vertices);
		SAFE_DELETE_ARRAY(indices);

		SAFE_DELETE(worldBuffer);
		SAFE_DELETE(shader);

	}
	/////////////////////////////////////초기화/////////////////////////////////////
	void SizeInit(float lengh)
	{
		vertexIdx = 0;
		lineLengh = lengh;
		vertexCount = indexCount = 2;

		SetColor(D3DXCOLOR(1, 0, 0, 1));
		SetLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0));
		SetVertexBuffer();
	}
	////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////초기화 하고 여분셋팅/////////////////////////////////////
	void SetLine(D3DXVECTOR3 startPos, D3DXVECTOR3 lineDir)
	{
		vertices[0].Position = startPos;
		vertices[0].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[1].Position =
			D3DXVECTOR3(
				lineDir.x*lineLengh + startPos.x, //버텍스 위치에서 일정방향 위치
				lineDir.y*lineLengh + startPos.y,
				lineDir.z*lineLengh + startPos.z
			);
		vertices[1].Color = D3DXCOLOR(1, 0, 0, 1);
	}
	void SetLine(D3DXVECTOR3 startPos, D3DXVECTOR3 lineDir, float line)
	{
		vertices[0].Position = startPos;
		vertices[0].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[1].Position =
			D3DXVECTOR3(
				lineDir.x*line + startPos.x, //버텍스 위치에서 일정방향 위치
				lineDir.y*line + startPos.y,
				lineDir.z*line + startPos.z
			);
		vertices[1].Color = D3DXCOLOR(1, 0, 0, 1);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////포지션이 직접적으로 변할때 업데이트에서 사용/////////////////////////
	D3DXVECTOR3 UpdateLine(D3DXVECTOR3 startPos, D3DXVECTOR3 lineDir, float line)
	{
		vertices[0].Position = startPos;
		vertices[0].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[1].Position =
			D3DXVECTOR3(
				lineDir.x*line + startPos.x, //버텍스 위치에서 일정방향 위치
				lineDir.y*line + startPos.y,
				lineDir.z*line + startPos.z
			);
		vertices[1].Color = D3DXCOLOR(1, 0, 0, 1);

		D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormal), vertexCount);

		return vertices[1].Position - vertices[0].Position;
	}
	void UpdateLine(D3DXVECTOR3 startPos, D3DXVECTOR3 endPos)
	{
		vertices[0].Position = startPos;
		vertices[0].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[1].Position = endPos;
		vertices[1].Color = D3DXCOLOR(1, 0, 0, 1);

		D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormal), vertexCount);
	}
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////랜더////////////////////////////////////////////
	void DrawLine()
	{
		UINT stride = sizeof(VertexColor);
		UINT offset = 0;
		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		worldBuffer->SetVSBuffer(1);
		shader->Render();
		lineColor->SetPSBuffer(10);
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
	////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////색상셋팅///////////////////////////////////////
	void SetColor(D3DXCOLOR color)
	{
		lineColor->Data.color = color;
	}
	////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////초기셋팅값에 메트릭스변환시킬때/////////////////////////////
	void World(D3DXMATRIX&matrix)
	{
		worldBuffer->SetMatrix(matrix);
	}
	////////////////////////////////////////////////////////////////////////////////////////
};
