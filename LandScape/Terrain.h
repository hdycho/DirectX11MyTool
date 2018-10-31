#pragma once
#define VERTEX VertexTextureNormalTangent

class Terrain
{
public:
	Terrain();
	~Terrain();

	UINT VertexCount() { return vertexCount; }
	void CopyVertices(void*vertices);

	UINT GetWidth() { return width; }
	UINT GetHeight() { return height; }

	Texture* HeightMap() { return heightMap; }

private:
	void ReadHeightMap(wstring file);
	void CalcNormalVector();
	void AlignVertexData();

private:
	Texture* heightMap;
	UINT width, height;

	VERTEX*vertices;
	UINT*indices;

	UINT vertexCount,indexCount;
};