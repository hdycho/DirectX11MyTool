#include "Framework.h"
#include "MeshGrid.h"

MeshGrid::MeshGrid(Material* material, UINT countX, UINT countZ, float sizeX, float sizeZ, float offsetU, float offsetV, Effect*lineShader)
	: Mesh(material, lineShader), countX(countX), countZ(countZ), sizeX(sizeX), sizeZ(sizeZ)
	, offsetU(offsetU), offsetV(offsetV)
{
	CreateData();
}

MeshGrid::~MeshGrid()
{

}

void MeshGrid::CreateData()
{
	UINT cX = countX + 1;
	UINT cZ = countZ + 1;

	float w = sizeX * 0.5f;
	float h = sizeZ * 0.5f;

	float x = sizeX / (cX - 1);
	float z = sizeZ / (cZ - 1);

	float u = offsetU / (cX - 1);
	float v = offsetV / (cZ - 1);

	vector<VertexTextureNormalTangent> vertices;
	for (UINT i = 0; i < cZ; i++)
	{
		float z1 = h - (float)i * z;

		for (UINT k = 0; k < cX; k++)
		{
			float x1 = -w + (float)k * x;

			VertexTextureNormalTangent vertex;
			vertex.Position = D3DXVECTOR3(x1, 0, z1);
			vertex.Uv = D3DXVECTOR2((float)k * u, (float)i * v);
			vertex.Normal = D3DXVECTOR3(0, 1, 0);
			vertex.Tangent = D3DXVECTOR3(1, 0, 0);

			vertices.push_back(vertex);
		}
	}

	min = vertices[0].Position;
	max = vertices[0].Position;
	for (size_t i = 1; i < vertices.size(); i++)
	{
		if (min.x > vertices[i].Position.x)
			min.x = vertices[i].Position.x;
		if (min.y > vertices[i].Position.y)
			min.y = vertices[i].Position.y;
		if (min.z > vertices[i].Position.z)
			min.z = vertices[i].Position.z;

		if (max.x < vertices[i].Position.x)
			max.x = vertices[i].Position.x;
		if (max.y < vertices[i].Position.y)
			max.y = vertices[i].Position.y;
		if (max.z < vertices[i].Position.z)
			max.z = vertices[i].Position.z;
	}

	float length = fabs(max.x - min.x);

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z)); // 왼쪽하단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z)); // 왼쪽상단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z)); // 오른쪽하단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z)); // 오른쪽상단앞쪽

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z + length)); // 왼쪽하단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z + length)); // 왼쪽상단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z + length)); // 오른쪽하단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z + length)); // 오른쪽상단뒷쪽


	this->vertices = new VertexTextureNormalTangent[vertices.size()];
	vertexCount = vertices.size();
	copy(vertices.begin(), vertices.end(), stdext::checked_array_iterator<VertexTextureNormalTangent *>(this->vertices, vertexCount));


	vector<UINT> indices;
	for (UINT i = 0; i < cZ - 1; i++)
	{
		for (UINT k = 0; k < cX - 1; k++)
		{
			indices.push_back(i * cX + k);
			indices.push_back(i * cX + k + 1);
			indices.push_back((i + 1) * cX + k);

			indices.push_back((i + 1) * cX + k);
			indices.push_back(i * cX + k + 1);
			indices.push_back((i + 1) * cX + k + 1);
		}
	}

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}


void MeshGrid::ColliderUpdate()
{

}
