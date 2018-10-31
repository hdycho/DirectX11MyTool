#pragma once
#define VERTEX VertexTextureNormalTangent

class QuardTree
{
public:
	static const UINT DivideCount; // ������ ����

private:
	struct NodeType; // ���� �Ӽ�
public:
	QuardTree(ExecuteValues*values, class Terrain*terrain,class Frustum*frustum);
	~QuardTree();

	void Update();
	void Render();

	bool Y(OUT D3DXVECTOR3 * out,NodeType*node);			
	void AdjustY(D3DXVECTOR3 & location);

	bool&IsPick() { return isPick; }

	D3DXVECTOR3&Scale() { return scale; }
	D3DXVECTOR3&Rotate() { return rotate; }
	D3DXVECTOR3&Position() { return position; }
	void UpdateMatrix();

	class Material*GetMaterial() { return material; }

	Texture* GetHeightMap() { return heightMap; }
private:
	void RenderNode(NodeType*node);
	void DeleteNode(NodeType*node);

	//���� �ִ�����(����) ���ϴ� �Լ�
	void CalcMeshDimensions(UINT vertexCount, float&centerX, float&centerZ, float&meshWidth);

	//�θ� ���κ��� �ڽĳ�带 ����� �ִ�� ���� �ڽĵ��� ���ؽ������� ���ۻ���
	void CreateTreeNode(struct NodeType*node, float positionX, float positionZ, float width);

	//�ش� ����� ��ġ�� ���Ե� �ﰢ������ ��ȯ
	UINT ContainTriangleCount(float positionX, float positionZ, float width);

	//�ﰢ���� �ش��忡 �����ִ��� �Ǵ�
	bool IsTriangleContained(UINT index, float positionX, float positionZ, float width);

	//����������� �׸������� ť�����
	void AddCube(D3DXVECTOR3 center, float width, float height, D3DXCOLOR color);

	float GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2);

	void CreateNormalData();

public:
	UINT DrawCount() { return drawCount; }
	int&RasterNum() { return rasterNum; }
	int&DimensionLevel() { return dimensionLevel; }
	NodeType*ParentNode() { return parent; }
private:
	Texture* heightMap;
	ExecuteValues*values;

	UINT triangleCount, drawCount;
	UINT width, height;
	VERTEX*vertices;

	NodeType*parent;
	class Frustum*frustum;

	WorldBuffer*worldBuffer;
	Material*material;

	RasterizerState*rasterizer[2];
	vector<class DebugCube*> cubes;

	bool isPick;

	//�ͷ��� ��Ʈ����
	D3DXMATRIX terrainMat;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotate;
	D3DXVECTOR3 position;

	/////////////////ImGui///////////////////
	int rasterNum = 1;
	int dimensionLevel = 0;
public:
	bool isVisible;
	bool isActive;
private:
	struct NodeType
	{
		float X, Z, Width;
		int TriangleCount;

		ID3D11Buffer*VertexBuffer;
		ID3D11Buffer*IndexBuffer;

		VERTEX*vertices;
		UINT*indices;

		NodeType*Childs[4];//�ڽ�����

		int Level; // �����ܰ�
	};
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
		}

		struct Struct
		{
			int UvSize;
			float Padding[3];

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
	LineBuffer*lineBuffer;				//10��
	PS_DetailBuffer*PS_detailBuffer;	//11��
	VS_DetailBuffer*VS_detailBuffer;	//12��
	BrushBuffer* brushBuffer;			//13��
	wstring name;
};
