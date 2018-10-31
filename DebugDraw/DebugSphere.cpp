#include "stdafx.h"
#include "DebugSphere.h"

DebugSphere::DebugSphere(float radius,D3DXVECTOR3 pos, D3DXVECTOR3 sacle)
	:DebugObj(1,1),width(12),height(18)
{
	material = new Material(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();
	//���ؽ�������
	{
		vertexCount = (width + 1)*(height + 1);
		vertices = new VertexColor[vertexCount];

		float r = radius;	//  ���� ������
		float PI = 3.141592f;
		float xzAngle = 2 * PI / width;	//xz������������� ��������
		float yAngle = PI / height;	//y���� �������� ��������

		float deltaYladian = 0;
		D3DXVECTOR3 startPos=pos;

		for (int h = 0; h <= height;h++) // ������������ �׸�����
		{
			float deltaXZladian = 0;
			for (int w = 0; w <= width; w++)
			{
				//3���� ������ �������̿�
				vertices[h*(width + 1) + w].Position.x = sinf(deltaYladian)*cosf(deltaXZladian)*r + startPos.x;
				vertices[h*(width + 1) + w].Position.y = cosf(deltaYladian)*r + startPos.y;
				vertices[h*(width + 1) + w].Position.z = sinf(deltaYladian)*sin(deltaXZladian)*r + startPos.z;


				vertices[h*(width + 1) + w].Color = D3DXCOLOR(1, 0, 1, 1);
				deltaXZladian += xzAngle;
			}

			deltaYladian += yAngle;
			if (deltaYladian > PI) // �׷����������� 180�� ������ 0���� �ʱ�ȭ�ٽ�
				deltaYladian = 0;
		}
	}

	//�ε��� ������
	{
		indexCount = width*height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (int z = 0; z < height; z++)
		{
			for (int x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1)*z + x;
				indices[index + 1] = (width + 1)*(z + 1) + x;
				indices[index + 2] = (width + 1)*z + x + 1;

				indices[index + 3] = (width + 1)*z + x + 1;
				indices[index + 4] = (width + 1)*(z + 1) + x;
				indices[index + 5] = (width + 1)*(z + 1) + x + 1;

				index += 6;
			}
		}
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

DebugSphere::~DebugSphere()
{
}


void DebugSphere::ChangeColor(D3DXCOLOR color)
{
	colorBuffer->Data.color = color;
}

void DebugSphere::Update(D3DXMATRIX parentMatrix)
{
	sphereWorld = parentMatrix;
}

void DebugSphere::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(10);
	material->PSSetBuffer();
	worldBuffer->SetMatrix(sphereWorld);
	worldBuffer->SetVSBuffer(1);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}
