#include "Framework.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad(Material * material, float width, float height, Effect*lineShader)
	: Mesh(material, lineShader), width(width), height(height)
{
	CreateData();
}

MeshQuad::~MeshQuad()
{

}

void MeshQuad::CreateData()
{
	vector<VertexTextureNormalTangent> vertices;

	float w = width * 0.5f;
	float h = height * 0.5f;

	vertices.push_back(VertexTextureNormalTangent(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	vertices.push_back(VertexTextureNormalTangent(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

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

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z)); // �����ϴܾ���
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z)); // ���ʻ�ܾ���
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z)); // �������ϴܾ���
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z)); // �����ʻ�ܾ���

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z + length)); // �����ϴܵ���
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z + length)); // ���ʻ�ܵ���
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z + length)); // �������ϴܵ���
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z + length)); // �����ʻ�ܵ���

	this->vertices = new VertexTextureNormalTangent[vertices.size()];
	vertexCount = vertices.size();
	copy
	(
		vertices.begin(), vertices.end(),
		stdext::checked_array_iterator<VertexTextureNormalTangent *>(this->vertices, vertexCount)
	);

	indexCount = 6;
	this->indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3
	};
}


void MeshQuad::ColliderUpdate()
{

}
