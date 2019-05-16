#pragma once

class TerrainCP
{
public:
	D3DXVECTOR3 Position;

	D3DXVECTOR2 Uv;
	D3DXVECTOR2 BoundsY;

	TerrainCP()
	{

	}

	TerrainCP(D3DXVECTOR3 pos, D3DXVECTOR2 uv, D3DXVECTOR2 boundsY)
	{
		Position = pos;
		Uv = uv;
		BoundsY = boundsY;
	}
};

class Patch
{
public:
	enum class NeighborDir { Top, Left };

public:
	Patch();
	~Patch();

	void InitPatchData(int patchWidth);

private:
	void BuildCenterIndices();
	void BuildTopEdges();
	void BuildLeftEdges();

private:
	WORD width;
	vector<TerrainCP> vertices;
	ID3D11Buffer* vertexBuffer;

	map<WORD, ID3D11Buffer*> indexBuffer;
	map<WORD, WORD> indexCount;
	map<WORD, ID3D11Buffer*> centerIndexBuffer;
	map<WORD, WORD> centerIndexCount;
	map< NeighborDir, map<tuple<WORD, WORD>, ID3D11Buffer*> > edgeIbs;
	map< NeighborDir, map<tuple<WORD, WORD>, WORD> > edgeIndicesCount;
};
