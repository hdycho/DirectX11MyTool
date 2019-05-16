#pragma once
#include "Interface\IPicked.h"
#include "Interface\IData.h"
#include "Interface\IShadow.h"

class Terrain :public IPicked, public IData, public IShadow
{
public:
	struct InitDesc;

public:
	Terrain(InitDesc& desc);
	Terrain(Terrain&copy);
	~Terrain();

	void Initialize();

	void Ready();

	void Update();
	void Render();
	void LineUpdate();

	void Data(UINT row, UINT col, float data);
	float Data(UINT row, UINT col);

	float Width();
	float Depth();

	float GetHeight(float x, float z);
	bool Picking(OUT D3DXVECTOR3 *pickPixel, OUT D3DXVECTOR3 * pickWorldPos);

	class HeightMap* GetHeightMap() { return heightMap; }
	class TerrainRender*GetTerrainRender() { return render; }

	//=======================인터페이스=================//
	wstring&Name() { return terrainName; }
	wstring&LoadDataFile() {return loadFileName;}
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }
	bool IsPicked();
	void SetPickState(bool val);
	class GData* Save();
	void Load(wstring fileName);
	void SetShader(D3DXMATRIX v, D3DXMATRIX p) override;
	void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv) override;
	void ShadowUpdate() override;
	void NormalRender() override;
	void ShadowRender(UINT tech, UINT pass) override;
	//=================================================//

public:
	//빌보드생성
	void CreateBillboard(class Shadow*shadow, Effect*lineEffect = NULL);
	class Billboard*GetBillboard() { return tree; }

public:
	static const UINT CellsPerPatch;

private:
	class TerrainRender* render;
	class HeightMap* heightMap;
	class Billboard*tree;

	wstring loadDataFile;

	wstring materialValueFile[4]; //머터리얼속성

public:
	struct InitDesc
	{
		wstring HeightMap;
		float HeightScale = 120.0f; // 높이에 대한 비율값
		float HeightMapWidth = 2049;
		float HeightMapHeight = 2049;

		wstring layerMapFile[3];
		float CellSpacing = 0.5f; // 한 셀의 크기

		D3DXVECTOR4 blendHeight = { 0.06f, 0.40f, 0.0f, 0.75f };
		D3DXVECTOR4 activeBlend = { 1,1,1,1 };
		Material* material;

		void HeightMapInit(wstring heightMap, float heightScale)
		{
			this->HeightMap = heightMap;
			this->HeightScale = heightScale;
		}

		void BlendMapInit(vector<wstring>&layerMap)
		{
			for (UINT i = 0; i < layerMap.size(); i++)
				layerMapFile[i] = layerMap[i];
		}

		void BlendMapInit(wstring layerMap, int index)
		{
			layerMapFile[index] = layerMap;
		}
	};
private:
	InitDesc desc;
	wstring terrainName;
	wstring loadFileName;
	bool isPick;

public:
	struct TerrainDetail
	{
		float DetailValue = 0.999f;
		float DetailIntensity = 1;
	}detail;

	struct TerrainBrush
	{
		int Type = 0;
		int Range = 10;
		D3DXVECTOR3 Color = { 1,0,0 };
		float BrushPower = 1;
	}brush;

	struct TerrainLine
	{
		int LineType = 0;
		D3DXVECTOR3 LineColor = { 1,1,1 };
		int Spacing = 5; // 한칸의 넓이
		float Thickness = 0.1f;
	}tline;

public:
	InitDesc& Desc() { return desc; }

private:
	//그림자
	ID3DX11EffectScalarVariable*lineTypeVariable;
	ID3DX11EffectScalarVariable*spacingVariable;
	ID3DX11EffectScalarVariable*thickVariable;
	ID3DX11EffectVectorVariable*lineColorVariable;

	//일단 안씀
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
};