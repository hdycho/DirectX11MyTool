#pragma once
#define VERTEX VertexTextureNormalTangent

class TerrainRender
{
public:
	TerrainRender(ExecuteValues*values);
	~TerrainRender();

	void Update();
	void Render();

	bool Y(OUT D3DXVECTOR3 * out);
	float Y(D3DXVECTOR3& position);
	void AdjustY(D3DXVECTOR3 & location);

	bool&IsPick() { return isPick; }

	UINT Width() { return width; }
	UINT Height() { return height; }
	Material*GetMaterial() { return material; }
	Texture* HeightMap() { return heightMap; }

	D3DXVECTOR3&Scale() { return scale; }
	D3DXVECTOR3&Rotate() { return rotate; }
	D3DXVECTOR3&Position() { return position; }
	void UpdateMatrix();
public:
	UINT DrawCount() { return drawCount; }
	int&RasterNum() { return rasterNum; }
private:
	void CreateNormalData();
	float GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2);
private:
	Texture* heightMap;
	ExecuteValues*values;

	Material* material;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	VERTEX* vertices;
	UINT* indices;

	UINT width, height, drawCount;
	UINT vertexCount, indexCount;

	int rasterNum = 1;
	bool isPick;

	RasterizerState*rasterizer[2];

	//터레인 메트릭스
	D3DXMATRIX terrainMat;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotate;
	D3DXVECTOR3 position;
private:
	class LineBuffer : public ShaderBuffer
	{
	public:
		LineBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 0;
			Data.Color = D3DXVECTOR3(1, 1, 1);

			Data.Spacing = 5;
			Data.Thickness = 0.1f;
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Color;
			int Spacing;
			float Thickness;

			float Padding[2];
		} Data;
	};

	class PS_DetailBuffer : public ShaderBuffer
	{
	public:
		PS_DetailBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.DetailPower = 2.5f;
			Data.Depth = { 0.99f,0.995f };
		}

		struct Struct
		{
			float DetailPower;
			D3DXVECTOR2 Depth;
			float Padding;

		} Data;
	};

	class VS_DetailBuffer : public ShaderBuffer
	{
	public:
		VS_DetailBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.UvSize = 1;
			Data.VertexDepth = { 0.99f,0.995f };
		}

		struct Struct
		{
			int UvSize;
			D3DXVECTOR2 VertexDepth;
			float Padding;

		} Data;
	};

	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 0;
			Data.Location = D3DXVECTOR3(0, 0, 0);
			Data.Range = 2;
			Data.Color = D3DXVECTOR3(0, 1, 0);
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Location;
			int Range;
			D3DXVECTOR3 Color;
		} Data;
	};
public:
	LineBuffer*lineBuffer;				//10번
	PS_DetailBuffer*PS_detailBuffer;	//11번
	VS_DetailBuffer*VS_detailBuffer;	//12번
	BrushBuffer* brushBuffer;			//13번
	wstring name;

	bool isVisible = true;
	bool isActive = true;
	int brushType=0;
	float brushPower = 20;
};