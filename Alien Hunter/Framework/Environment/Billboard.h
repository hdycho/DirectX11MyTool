#pragma once
#include "Interface\IPicked.h"
#include "Interface\IData.h"
#include "Interface\IShadow.h"
#include "Interface\ICollider.h"

class Billboard :
	public IData,
	public IShadow,
	public ICollider
{
public:
	enum class BillboardType
	{
		ONE,	//	�Ѱ����
		TWO = 1,	//	�ΰ����
		FOUR = 3
	}bType;

	enum TextureType
	{
		Tree1 = 0,
		Tree2 = 1,
		Tree3 = 2
	};

public:
	Billboard(class Terrain*terrain, Effect*lineEffect = NULL);
	Billboard(Billboard&copy);
	~Billboard();

	void Initalize();
	void Ready();
	void Render();
	void Update();
	void ImGuiRender();

	void BuildBillboard(D3DXVECTOR3 pos);
	void RemoveBillboard();
	void ResourceUpdate(D3DXVECTOR2 setScale); // �������̳� �ؽ��Ĺٲܶ����

											   //=======================�������̽�=================//
	wstring&Name() { return billboardName; }
	wstring&LoadDataFile() { return loadFileName; }
	class GData* Save() { return NULL; }
	void Load(wstring fileName);
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }

	void SetShader(D3DXMATRIX v, D3DXMATRIX p) override;
	void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv) override;
	void ShadowUpdate() override;
	void NormalRender() override;
	void ShadowRender(UINT tech, UINT pass) override;

	void CreateCollider(int type) {}; // ������ �����ǹڽ��� �����
	void ColliderUpdate();
	vector<ColliderElement*>&GetColliders() { return noneColiders; } // �ϴ� ��ȯ����
																				   //=================================================//

private:
	struct InstanceBillboard
	{
		D3DXVECTOR3 TransPosition;
		D3DXMATRIX DeTransWorld;
		D3DXMATRIX RotateWorld;
		D3DXVECTOR2 Size;
		UINT TextureNumber;
	};

private:
	//�ӱ��� ��ǥ��������
	UINT setTextureNumber;
	D3DXVECTOR2 setSize = { 10,10 };

	vector<Texture*> textures;

	TextureType treeType = (TextureType)0;

private:
	vector<ColliderElement*> noneColiders; // �̰� ��ȯ�ҰǾ��� ���ߴ°� ���ٶ��
	vector<InstanceBillboard> vertices;

	class Frustum* frustum;

	Vertex vertex;
	ID3D11Buffer*vertexBuffer[2];

	UINT BillboardCount;

	Material*BillboardMaterial;
	TextureArray*billboardTexture;
	vector<wstring> textureNames;

	class Terrain*terrain;
	wstring billboardName;
	wstring loadFileName;

private:
	D3DXPLANE plane[6];

private:
	Effect*lineEffect;

	struct BCollider
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 scale;
		class ColliderBox*Box;
	};
	vector<BCollider> billboardCollider;

public:
	vector<InstanceBillboard>&GetVertices() { return vertices; }
	vector<BCollider>&GetBillboardCollider() { return billboardCollider; }
	void SetLineEffect(Effect*lineEffect)// �ε�� �����彦�̴������Ҷ� �ʿ�
	{
		this->lineEffect = lineEffect;
	}

private:
	ID3DX11EffectShaderResourceVariable*srvVariable;
	ID3DX11EffectScalarVariable*fogStartVariable;
	ID3DX11EffectScalarVariable*fogRangeVariable;
	ID3DX11EffectVectorVariable*frustumVariable;

	//�ϴ� �Ⱦ�
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
};
