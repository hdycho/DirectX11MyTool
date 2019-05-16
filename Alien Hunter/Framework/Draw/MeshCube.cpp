#include "Framework.h"
#include "MeshCube.h"

MeshCube::MeshCube(Material * material, float width, float height, float depth, Effect*lineShader)
	: Mesh(material, lineShader), width(width), height(height), depth(depth)
{
	CreateData();
}

MeshCube::~MeshCube()
{

}

void MeshCube::CreateData()
{
	vector<VertexTextureNormalTangent> vertices;

	float w = width * 0.5f;
	float h = height * 0.5f;
	float d = depth * 0.5f;

	//Front
	vertices.push_back(VertexTextureNormalTangent(-w, -h, -d, 0, 1, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, -d, 0, 0, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, -d, 1, 0, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, -d, 1, 1, 0, 0, -1, 1, 0, 0));

	//Back
	vertices.push_back(VertexTextureNormalTangent(-w, -h, +d, 1, 1, 0, 0, 1, -1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, +d, 0, 1, 0, 0, 1, -1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, +d, 0, 0, 0, 0, 1, -1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, +d, 1, 0, 0, 0, 1, -1, 0, 0));

	//Top
	vertices.push_back(VertexTextureNormalTangent(-w, +h, -d, 0, 1, 0, 1, 0, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, +d, 0, 0, 0, 1, 0, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, +d, 1, 0, 0, 1, 0, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, -d, 1, 1, 0, 1, 0, 1, 0, 0));

	//Bottom
	vertices.push_back(VertexTextureNormalTangent(-w, -h, -d, 0, -1, 0, -1, 0, 0, 1, 1));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, -d, 0, -1, 0, -1, 0, 0, 0, 1));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, +d, 0, -1, 0, -1, 0, 0, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(-w, -h, +d, 0, -1, 0, -1, 0, 0, 1, 0));

	//Left
	vertices.push_back(VertexTextureNormalTangent(-w, -h, +d, 0, 1, -1, 0, 0, 0, 0, -1));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, +d, 0, 0, -1, 0, 0, 0, 0, -1));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, -d, 1, 0, -1, 0, 0, 0, 0, -1));
	vertices.push_back(VertexTextureNormalTangent(-w, -h, -d, 1, 1, -1, 0, 0, 0, 0, -1));

	//Right
	vertices.push_back(VertexTextureNormalTangent(+w, -h, -d, 0, 1, 1, 0, 0, 0, 0, 1));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, -d, 0, 0, 1, 0, 0, 0, 0, 1));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, +d, 1, 0, 1, 0, 0, 0, 0, 1));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, +d, 1, 1, 1, 0, 0, 0, 0, 1));

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
	copy
	(
		vertices.begin(), vertices.end(),
		stdext::checked_array_iterator<VertexTextureNormalTangent *>(this->vertices, vertexCount)
	);


	indexCount = 36;
	this->indices = new UINT[indexCount]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
}


void MeshCube::ColliderUpdate()
{
	if (Name() == L"Cube0")
	{
		for (size_t i = 0; i < colliders.size(); i++)
		{
			CollisionResult * result = CollisionContext::Get()->HitTest(colliders[i], CollisionContext::Get()->GetLayer(L"Collision Enemy"), CollisionResult::ResultType::ENearestOne);

			if (result != NULL)
			{
				Position(-50, 0, 0);
			}
		}
	}
}
