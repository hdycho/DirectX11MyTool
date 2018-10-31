#include "stdafx.h"
#include "QuardTree.h"
#include "Terrain.h"
#include "Viewer\Frustum.h"
#include "DebugDraw\DebugCube.h"

const UINT QuardTree::DivideCount = 1024; // 32x32

QuardTree::QuardTree(ExecuteValues*values, Terrain*terrain,Frustum*frustum)
	:values(values), drawCount(0),frustum(frustum)
{
	material = new Material(Shaders + L"032_DetailTerrain.hlsl");
	material->SetDiffuseMap(Textures + L"Dirt3.png");
	material->SetNormalMap(Textures + L"Dirt2_normal.png");
	material->SetDetailMap(Textures + L"Dirt2_detail.png");

	worldBuffer = new WorldBuffer();
	lineBuffer = new LineBuffer();
	PS_detailBuffer = new PS_DetailBuffer();
	VS_detailBuffer = new VS_DetailBuffer();
	brushBuffer = new BrushBuffer();

	D3DXMatrixIdentity(&terrainMat);
	position = { 0,0,0 };
	rotate = { 0,0,0 };
	scale = { 1,1,1 };

	UINT vertexCount = terrain->VertexCount();
	triangleCount = vertexCount / 3;

	vertices = new VERTEX[vertexCount];
	terrain->CopyVertices((void*)vertices);

	width = terrain->GetWidth();
	height = terrain->GetHeight();

	heightMap = terrain->HeightMap();

	float centerX = 0.0f, centerZ = 0.0f, width = 0.0f;
	//매쉬의 최대 직경(지름)계산
	CalcMeshDimensions(vertexCount, centerX, centerZ, width);


	parent = new NodeType();
	parent->Level = 0;
	CreateTreeNode(parent, centerX, centerZ, width);

	rasterizer[0] = new RasterizerState;
	rasterizer[0]->FillMode(D3D11_FILL_WIREFRAME);

	rasterizer[1] = new RasterizerState;
	rasterizer[1]->FillMode(D3D11_FILL_SOLID);

	SAFE_DELETE_ARRAY(vertices);

	isPick = false;
	isVisible = true;
	isActive = true;
}

QuardTree::~QuardTree()
{
	DeleteNode(parent);
	SAFE_DELETE(parent);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(lineBuffer);
	SAFE_DELETE(PS_detailBuffer);
	SAFE_DELETE(VS_detailBuffer);
	SAFE_DELETE(material);
	SAFE_DELETE(heightMap);
}

void QuardTree::Update()
{

}

void QuardTree::Render()
{
	if (isVisible)
	{
		drawCount = 0;
		D3DXCOLOR checkColor(0, 0, 0, 0);

		if (dimensionLevel == 1)
			checkColor = { 1, 0, 0, 1 };
		else if (dimensionLevel == 2)
			checkColor = { 0, 1, 0, 1 };
		else if (dimensionLevel == 3)
			checkColor = { 0, 0, 1, 1 };
		else if (dimensionLevel == 4)
			checkColor = { 0, 1, 1, 1 };

		for (DebugCube*cube : cubes)
		{
			if (cube->GetColor() == checkColor)
			{
				if (frustum->ContainCube(cube->GetPosition(), cube->GetLength().x) == true)
				{
					cube->Render();
				}
			}
		}
		RenderNode(parent);


		//TODO: 프러스텀나중에 툴쪽으로 빼자
		//frustum->Render();
	}
}

bool QuardTree::Y(OUT D3DXVECTOR3 * out,NodeType*node)
{
	D3DXVECTOR3 center(node->X, 0, node->Z);
	float d = node->Width / 2.0f;

	if (frustum->ContainCube(center, d) == false)
		return false;
	UINT count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Childs[i] != NULL)
		{
			count++;
			Y(out,node->Childs[i]);
		}
	}
	// 자식이 있으면 부모는 그릴 필요없어서 pass 시킨거
	if (count != 0)
		return false;
	//더이상 자식이 없으면(삼각형갯수16x16) 피킹시작

	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXMATRIX initWorld;
	D3DXMatrixIdentity(&initWorld);
	D3DXMATRIX inverse;
	D3DXMatrixInverse(&inverse, NULL, &initWorld);

	D3DXVECTOR3 direction = values->MainCamera->Direction
	(
		values->Viewport, values->Perspective
	);

	D3DXVec3TransformNormal(&direction, &direction, &inverse);
	D3DXVec3Normalize(&direction, &direction);

	for (UINT idx = 0; idx < node->TriangleCount*3; idx+=6)
	{
		UINT index[6];
		for (int a = 0; a < 6; a++)
			index[a] = idx + a;

		D3DXVECTOR3 p[6];
		for (int i = 0; i < 6; i++)
			p[i] = node->vertices[index[i]].Position;

		float u, v, distance;
		if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
		{
			*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
			brushBuffer->Data.Location = *out;

			return true;
		}

		if (D3DXIntersectTri(&p[3], &p[4], &p[5], &start, &direction, &u, &v, &distance))
		{
			*out = p[3] + (p[4] - p[3]) * u + (p[5] - p[3]) * v;
			brushBuffer->Data.Location = *out;

			return true;
		}
	}//for(x)


	return false;
}

void QuardTree::AdjustY(D3DXVECTOR3 & location)
{
	//switch (brushBuffer->Data.Type)
	//{
	//	case 1:
	//	{
	//		UINT size = (UINT)brushBuffer->Data.Range;

	//		D3D11_BOX box;
	//		box.left = (UINT)location.x - size;
	//		box.top = (UINT)location.z + size;
	//		box.right = (UINT)location.x + size;
	//		box.bottom = (UINT)location.z - size;

	//		if (box.left < 0)box.left = 0;
	//		if (box.top >= height)box.top = height;
	//		if (box.right >= width)box.right = width;
	//		if (box.bottom < 0)box.bottom = 0;

	//		for (UINT z = box.bottom; z <= box.top; z++)
	//		{
	//			for (UINT x = box.left; x <= box.right; x++)
	//			{
	//				UINT index = (width + 1) * z + x;

	//				vertices[index].Position.y += 20.0f * Time::Delta();
	//			}
	//		}
	//		CreateNormalData();
	//	}
	//	break;
	//	case 2:
	//	{
	//		UINT size = (UINT)brushBuffer->Data.Range; // 반지름

	//												   //중점설정
	//		D3DXVECTOR3 centerPos = location;

	//		D3D11_BOX box;
	//		box.left = (UINT)location.x - size;
	//		box.top = (UINT)location.z + size;
	//		box.right = (UINT)location.x + size;
	//		box.bottom = (UINT)location.z - size;

	//		if (box.left < 0)box.left = 0;
	//		if (box.top >= height)box.top = height;
	//		if (box.right >= width)box.right = width;
	//		if (box.bottom < 0)box.bottom = 0;

	//		for (UINT z = box.bottom; z <= box.top; z++)
	//		{
	//			for (UINT x = box.left; x <= box.right; x++)
	//			{
	//				UINT index = (width + 1) * z + x;
	//				if (GetDistance(vertices[index].Position, centerPos) <= size)
	//				{
	//					float x = GetDistance(vertices[index].Position, centerPos) / size;
	//					x = x * (float)D3DX_PI / 2.0f;
	//					x = cosf(x);
	//					vertices[index].Position.y += 0.2f*x;
	//				}
	//			}
	//		}
	//		CreateNormalData();
	//	}
	//	break;
	//	case 3:
	//	{
	//		UINT size = (UINT)brushBuffer->Data.Range;
	//		float dx, dz, dist, ddist;

	//		D3D11_BOX box;
	//		box.left = (UINT)location.x - size;
	//		box.top = (UINT)location.z + size;
	//		box.right = (UINT)location.x + size;
	//		box.bottom = (UINT)location.z - size;

	//		if (box.left < 0)box.left = 0;
	//		if (box.top >= height)box.top = height;
	//		if (box.right >= width)box.right = width;
	//		if (box.bottom < 0)box.bottom = 0;

	//		for (int z = box.bottom; z <= box.top; z++)
	//		{
	//			for (int x = box.left; x <= box.right; x++)
	//			{
	//				int idx = (width + 1)*z + x;
	//				dx = vertices[idx].Position.x - location.x;
	//				dz = vertices[idx].Position.z - location.z;
	//				dist = sqrt(dx * dx + dz * dz);
	//				if (dist > size) continue;
	//				dist = (size - dist) / (float)size * (D3DX_PI / 2.0f);
	//				vertices[idx].Position.y += ((sinf(dist) * (float)size)) * Time::Delta();
	//			}
	//		}
	//		CreateNormalData();
	//	}
	//	break;
	//}
	//D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormal), vertexCount);
}

void QuardTree::UpdateMatrix()
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

void QuardTree::RenderNode(NodeType * node)
{
	D3DXVECTOR3 center(node->X, 0, node->Z);
	float d = node->Width / 2.0f;

	if (frustum->ContainCube(center, d) == false)
		return;
	UINT count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Childs[i] != NULL)
		{
			count++;
			RenderNode(node->Childs[i]);
		}
	}
	// 자식이 있으면 부모는 그릴 필요없어서 pass 시킨거
	if (count != 0)
		return;
	//더이상 자식이 없으면(삼각형갯수16x16) 랜더시작
	
	worldBuffer->SetMatrix(terrainMat);
	worldBuffer->SetVSBuffer(1);
	material->PSSetBuffer();
	lineBuffer->SetPSBuffer(10);
	PS_detailBuffer->SetPSBuffer(11);
	VS_detailBuffer->SetVSBuffer(12);
	brushBuffer->SetVSBuffer(13);

	rasterizer[rasterNum]->RSSetState();

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &node->VertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(node->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT indexCount = node->TriangleCount * 3;
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	drawCount += node->TriangleCount;
}

void QuardTree::DeleteNode(NodeType * node)
{
	for (int i = 0; i < 4; i++)
	{
		if (node->Childs[i] != NULL)
			DeleteNode(node->Childs[i]);

		SAFE_DELETE(node->Childs[i]);
	}

	SAFE_RELEASE(node->VertexBuffer);
	SAFE_RELEASE(node->IndexBuffer);
}

void QuardTree::CalcMeshDimensions(UINT vertexCount, float & centerX, float & centerZ, float & meshWidth)
{
	centerX = centerZ = 0.0f;

	for (UINT i = 0; i < vertexCount; i++)
	{
		centerX += vertices[i].Position.x;
		centerZ += vertices[i].Position.z;
	}

	//큰 면의 중심점을 먼저구한다 => 중간값
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;


	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(vertices[0].Position.x - centerX); // 반지름길이(가로)
	float minDepth = fabsf(vertices[0].Position.z - centerZ); // 반지름길이(세로)

	for (UINT i = 0; i < vertexCount; i++)
	{
		float width = fabsf(vertices[i].Position.x - centerX);
		float depth = fabsf(vertices[i].Position.z - centerZ);

		if (width > maxWidth)maxWidth = width;
		if (depth > maxDepth)maxDepth = depth;
		if (width < minWidth)minWidth = width;
		if (depth < minDepth)minDepth = depth;
	}

	float maxX = (float)max(fabsf(minWidth), fabsf(maxWidth));
	float maxZ = (float)max(fabsf(minDepth), fabsf(maxDepth));
	// mesh 최대 직경 계산
	meshWidth = (float)max(maxX, maxZ)*2.0f;
}

void QuardTree::CreateTreeNode(NodeType * node, float positionX, float positionZ, float width)
{
	if (node->Level == 0) // 부모노드일때
		node->Level += 1; // 한단계더해준다

	node->X = positionX;
	node->Z = positionZ;
	node->Width = width;

	node->TriangleCount = 0;

	node->VertexBuffer = NULL;
	node->IndexBuffer = NULL;

	for (UINT i = 0; i < 4; i++)
		node->Childs[i] = NULL;

	//해당노드(면)에 포함된 삼각형 갯수
	UINT triangles = ContainTriangleCount(positionX, positionZ, width);

	//Case 1 : 남은 갯수가 없을때
	if (triangles == 0)
		return;

	//Case 2 : 더 작은 노드로 분할
	if (triangles > DivideCount)
	{
		for (UINT i = 0; i < 4; i++)
		{
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			UINT count = ContainTriangleCount((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));

			D3DXVECTOR3 centerPos = { (positionX + offsetX) ,0,(positionZ + offsetZ) };

			if (node->Level == 1)
				AddCube(centerPos, (width / 4.0f), (width / 4.0f), D3DXCOLOR(1, 0, 0, 1));
			if (node->Level == 2)
				AddCube(centerPos, (width / 4.0f), (width / 4.0f), D3DXCOLOR(0, 1, 0, 1));
			else if (node->Level == 3)
				AddCube(centerPos, (width / 4.0f), (width / 4.0f), D3DXCOLOR(0, 0, 1, 1));
			else if (node->Level == 4)
				AddCube(centerPos, (width / 4.0f), (width / 4.0f), D3DXCOLOR(0, 1, 1, 1));

			if (count > 0)
			{
				node->Childs[i] = new NodeType();
				node->Childs[i]->Level = node->Level + 1;

				CreateTreeNode(node->Childs[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}
		return;
	}

	//Case 3 : 남은 갯수가 있을때 => 16x16으로 줄었을때
	node->TriangleCount = triangles;
	UINT vertexCount = triangles * 3;
	//VERTEX*vertices = new VERTEX[vertexCount];
	//UINT*indices = new UINT[vertexCount];

	node->vertices = new VERTEX[vertexCount];
	node->indices = new UINT[vertexCount];

	UINT index = 0, vertexIndex = 0;
	for (UINT i = 0; i < triangleCount; i++)
	{
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			vertexIndex = i * 3;

			node->vertices[index].Position = this->vertices[vertexIndex].Position;
			node->vertices[index].Uv = this->vertices[vertexIndex].Uv;
			node->vertices[index].Normal = this->vertices[vertexIndex].Normal;
			node->indices[index] = index;
			index++;

			vertexIndex++;
			node->vertices[index].Position = this->vertices[vertexIndex].Position;
			node->vertices[index].Uv = this->vertices[vertexIndex].Uv;
			node->vertices[index].Normal = this->vertices[vertexIndex].Normal;
			node->indices[index] = index;
			index++;

			vertexIndex++;
			node->vertices[index].Position = this->vertices[vertexIndex].Position;
			node->vertices[index].Uv = this->vertices[vertexIndex].Uv;
			node->vertices[index].Normal = this->vertices[vertexIndex].Normal;
			node->indices[index] = index;
			index++;
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VERTEX) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &node->vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data,
			&node->VertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * vertexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &node->indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data,
			&node->IndexBuffer);
		assert(SUCCEEDED(hr));
	}

	//SAFE_DELETE_ARRAY(vertices);
	//SAFE_DELETE_ARRAY(indices);
}

UINT QuardTree::ContainTriangleCount(float positionX, float positionZ, float width)
{
	UINT count = 0;

	for (int i = 0; i < triangleCount; i++)
	{
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
			count++;
	}

	return count;
}

bool QuardTree::IsTriangleContained(UINT index, float positionX, float positionZ, float width)
{
	float radius = width / 2.0f;	//  반지름
	UINT vertexIndex = index * 3;	//	삼각형3개를 처리하기위해

	float x1 = vertices[vertexIndex].Position.x;
	float z1 = vertices[vertexIndex].Position.z;
	vertexIndex++;

	float x2 = vertices[vertexIndex].Position.x;
	float z2 = vertices[vertexIndex].Position.z;
	vertexIndex++;

	float x3 = vertices[vertexIndex].Position.x;
	float z3 = vertices[vertexIndex].Position.z;

	//면의 최대최소 길이로 포함된 삼각형을 판단
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
		return false;

	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
		return false;

	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ >(positionZ + radius))
		return false;

	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
		return false;

	return true;
}

void QuardTree::AddCube(D3DXVECTOR3 center, float width, float height, D3DXCOLOR color)
{
	DebugCube*cube = new DebugCube(center, width, height, 20);
	cube->ChangeColor(color);
	cubes.push_back(cube);
}

float QuardTree::GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	float d1 = v1.x - v2.x;
	float d2 = v1.z - v2.z;

	float distance = sqrt(d1*d1 + d2*d2);

	return distance;
}

void QuardTree::CreateNormalData()
{
	
}


