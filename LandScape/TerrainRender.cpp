#include "stdafx.h"
#include "TerrainRender.h"

TerrainRender::TerrainRender(ExecuteValues*values)
	:values(values)
{
	isPick = false;
	heightMap = new Texture(Contents + L"HeightMaps/HeightMap256.png");

	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	//width = heightMap->GetWidth() - 1;
	//height = heightMap->GetHeight() - 1;

	width =255;
	height = 255;

	material = new Material(Shaders + L"032_DetailTerrain.hlsl");
	material->SetDiffuseMap(Textures + L"Dirt3.png");
	material->SetNormalMap(Textures + L"Dirt2_normal.png");
	material->SetDetailMap(Textures + L"Dirt2_detail.png");
	material->SetDiffuse(D3DXCOLOR(1, 1, 1, 1));

	D3DXMatrixIdentity(&terrainMat);
	position = { 0,0,0 };
	rotate = { 0,0,0 };
	scale = { 1,1,1 };

	rasterizer[0] = new RasterizerState;
	rasterizer[0]->FillMode(D3D11_FILL_WIREFRAME);

	rasterizer[1] = new RasterizerState;
	rasterizer[1]->FillMode(D3D11_FILL_SOLID);

	//Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VERTEX[vertexCount];

		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / (float)width;
				vertices[index].Uv.y = (float)z / (float)height;
			}
		}//for(z)
	}

	//CreateIndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * z + x; //0
				indices[index + 1] = (width + 1) * (z + 1) + x; //1
				indices[index + 2] = (width + 1) * z + x + 1; //2

				indices[index + 3] = (width + 1) * z + x + 1; //2
				indices[index + 4] = (width + 1) * (z + 1) + x; //1
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; //1

				index += 6;
			}
		}//for(z)
	}

	//CreateNormal
	{
		for (UINT i = 0; i < (indexCount / 3); i++)
		{
			UINT index0 = indices[i * 3 + 0];
			UINT index1 = indices[i * 3 + 1];
			UINT index2 = indices[i * 3 + 2];

			VERTEX v0 = vertices[index0];
			VERTEX v1 = vertices[index1];
			VERTEX v2 = vertices[index2];

			D3DXVECTOR3 d1 = v1.Position - v0.Position;
			D3DXVECTOR3 d2 = v2.Position - v0.Position;

			D3DXVECTOR3 normal;
			D3DXVec3Cross(&normal, &d1, &d2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}

		for (UINT i = 0; i < vertexCount; i++)
			D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VERTEX) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
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

	worldBuffer = new WorldBuffer();
	lineBuffer = new LineBuffer();
	PS_detailBuffer = new PS_DetailBuffer();
	VS_detailBuffer = new VS_DetailBuffer();
	brushBuffer = new BrushBuffer();
}

TerrainRender::~TerrainRender()
{
	SAFE_DELETE(material);
	SAFE_DELETE(heightMap);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(lineBuffer);
	SAFE_DELETE(PS_detailBuffer);
	SAFE_DELETE(VS_detailBuffer);
}

void TerrainRender::Update()
{
}

void TerrainRender::Render()
{
	if (isVisible)
	{
		rasterizer[rasterNum]->RSSetState();

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		worldBuffer->SetMatrix(terrainMat);
		worldBuffer->SetVSBuffer(1);
		material->PSSetBuffer();
		lineBuffer->SetPSBuffer(10);
		PS_detailBuffer->SetPSBuffer(11);
		VS_detailBuffer->SetVSBuffer(12);
		brushBuffer->SetVSBuffer(13);

		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
}

float TerrainRender::Y(D3DXVECTOR3& position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return 0.0f;
	if (z < 0 || z >= height) return 0.0f;

	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;

	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	//	TODO : 3D API.com

	return temp.y;
}

bool TerrainRender::Y(OUT D3DXVECTOR3 * out)
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXMATRIX initWorld;
	D3DXMatrixIdentity(&initWorld);

	D3DXVECTOR3 direction = values->MainCamera->Direction
	(
		values->Viewport, values->Perspective
	);

	D3DXMatrixIdentity(&initWorld);
	D3DXMATRIX inverse;
	D3DXMatrixInverse(&inverse, NULL, &initWorld);

	D3DXVec3TransformNormal(&direction, &direction, &inverse);
	D3DXVec3Normalize(&direction, &direction);

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index[4];
			index[0] = (width + 1) * z + x;
			index[1] = (width + 1) * (z + 1) + x;
			index[2] = (width + 1) * z + (x + 1);
			index[3] = (width + 1) * (z + 1) + x + 1;

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
				brushBuffer->Data.Location = *out;

				return true;
			}

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
				brushBuffer->Data.Location = *out;

				return true;
			}
		}//for(x)
	}//for(z);

	return false;
}

void TerrainRender::AdjustY(D3DXVECTOR3 & location)
{
	switch (brushBuffer->Data.Type)
	{
		case 1:
		{
			UINT size = (UINT)brushBuffer->Data.Range;

			D3D11_BOX box;
			box.left = (UINT)location.x - size;
			box.top = (UINT)location.z + size;
			box.right = (UINT)location.x + size;
			box.bottom = (UINT)location.z - size;

			if (box.left < 0)box.left = 0;
			if (box.top >= height)box.top = height;
			if (box.right >= width)box.right = width;
			if (box.bottom < 0)box.bottom = 0;

			for (UINT z = box.bottom; z <= box.top; z++)
			{
				for (UINT x = box.left; x <= box.right; x++)
				{
					UINT index = (width + 1) * z + x;

					if(brushType==0)
						vertices[index].Position.y += brushPower * Time::Delta();
					else if(brushType==1)
						vertices[index].Position.y -= brushPower * Time::Delta();
				}
			}
			CreateNormalData();
		}
		break;
		case 2:
		{
			UINT size = (UINT)brushBuffer->Data.Range; // 반지름

			//중점설정
			D3DXVECTOR3 centerPos = location;

			D3D11_BOX box;
			box.left = (UINT)location.x - size;
			box.top = (UINT)location.z + size;
			box.right = (UINT)location.x + size;
			box.bottom = (UINT)location.z - size;

			if (box.left < 0)box.left = 0;
			if (box.top >= height)box.top = height;
			if (box.right >= width)box.right = width;
			if (box.bottom < 0)box.bottom = 0;

			for (UINT z = box.bottom; z <= box.top; z++)
			{
				for (UINT x = box.left; x <= box.right; x++)
				{
					UINT index = (width + 1) * z + x;
					if (GetDistance(vertices[index].Position, centerPos) <= size)
					{
						float x = GetDistance(vertices[index].Position, centerPos) / size;
						x = x * (float)D3DX_PI / 2.0f;
						x = cosf(x);

						if (brushType == 0)
							vertices[index].Position.y += (brushPower*0.1f)*0.2f*x;
						else if (brushType==1)
							vertices[index].Position.y -= (brushPower*0.1f)*0.2f*x;
					}
				}
			}
			CreateNormalData();
		}
		break;
		case 3:
		{
			UINT size = (UINT)brushBuffer->Data.Range;
			float dx, dz, dist, ddist;

			D3D11_BOX box;
			box.left = (UINT)location.x - size;
			box.top = (UINT)location.z + size;
			box.right = (UINT)location.x + size;
			box.bottom = (UINT)location.z - size;

			if (box.left < 0)box.left = 0;
			if (box.top >= height)box.top = height;
			if (box.right >= width)box.right = width;
			if (box.bottom < 0)box.bottom = 0;

			for (int z = box.bottom; z <= box.top; z++)
			{
				for (int x = box.left; x <= box.right; x++)
				{
					int idx = (width + 1)*z + x;
					dx = vertices[idx].Position.x - location.x;
					dz = vertices[idx].Position.z - location.z;
					dist = sqrt(dx * dx + dz * dz);
					if (dist > size) continue;
					dist = (size - dist) / (float)size * (D3DX_PI / 2.0f);

					if (brushType == 0)
						vertices[idx].Position.y += (brushPower*0.1f)*((sinf(dist) * (float)size)) * Time::Delta();
					else if (brushType == 1)
						vertices[idx].Position.y -= (brushPower*0.1f)*((sinf(dist) * (float)size)) * Time::Delta();
				}
			}
			CreateNormalData();
		}
		break;
	}
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VERTEX), vertexCount);
}

void TerrainRender::UpdateMatrix()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	//   짐벌락 해결 방법 
	//   1. YawPitchRoll 함번에 처리
	//   2. 사원수 사용(Quaterion) :  
	//   TODO : Quaterion 찾아보기
	D3DXMatrixRotationYawPitchRoll(&R, rotate.y, rotate.x, rotate.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	terrainMat = S * R * T;
}

void TerrainRender::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VERTEX v0 = vertices[index0];
		VERTEX v1 = vertices[index1];
		VERTEX v2 = vertices[index2];

		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}



float TerrainRender::GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	float d1 = v1.x - v2.x;
	float d2 = v1.z - v2.z;

	float distance = sqrt(d1*d1 + d2*d2);

	return distance;
}
