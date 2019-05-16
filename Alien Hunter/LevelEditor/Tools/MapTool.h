#pragma once
#include "ToolHeader\GameTool.h"

class MapTool :public GameTool
{
private:
	enum MAP_BRUSH
	{
		None,
		UpBrush,
		DownBrush,
		BlendBrush
	};
	enum MAP_SETTYPE
	{
		SET_NONE,
		SET_DETAIL_TERRAIN,
		SET_HEIGHT_TERRAIN,
		SET_BRUSH_TERRAIN,
		SET_BLEND_TERRAIN,
		SET_FOG_TERRAIN,
		SET_BILLBOARD_TERRAIN
	};
	enum MAP_BLENDTYPE
	{
		BLEND1,
		BLEND2,
		BLEND3
	};

	struct AssetLoadType
	{
		bool showTerrain;
		bool showSky;
		bool showRain;

		AssetLoadType()
		{
			showTerrain = false;
			showSky = false;
			showRain = false;
		}
	}aType;
private:
	//지형관리
	typedef map<wstring, class Terrain*>::iterator terrainIter;
	typedef map<wstring, class CubeSky*>::iterator cubeSkyIter;
	typedef map<wstring, class Sky*>::iterator SkyIter;
	typedef map<wstring, class Rain*>::iterator rainIter;
public:
	MapTool();
	~MapTool();

	void Ready() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Update() override;

private:
	void PickedInit();			//피킹전부false로
								//void PickSetting(bool value);	//피킹끄고키기

	void DrawAsset() override;
	void DrawInspector() override;
	void DrawHiarachy() override;

	void CheckSelectObject();

	void LoadTerrainMapFile(class TerrainRender * tree, wstring filename, int type);
	void LoadHeightMapFile(class Terrain*terrain, wstring file);
	void LoadBlendMapFile(class Terrain*terrain, wstring file, int bNum);

	void LoadSkyMapFile(class Sky*sky, wstring file, int type);
	void LoadCubeSkyMapFile(class CubeSky*sky, wstring file);

	void CreateComputeTexture();
	void CheckSelectType();
	void TerrainTypeRender(class Terrain*terrain);

	void LoadMapData(wstring fileName);
private:
	//=============맵툴==============//
	Effect*brushEffect;
	ID3D11ShaderResourceView*srv;
	ID3D11UnorderedAccessView*uav;

	wstring selectType;
	vector<wstring> typeNames;

	Texture*layerImg[3];
	//==============================//

	//==========Enum타입============//
	MAP_SETTYPE mSetType = SET_NONE;
	MAP_BRUSH mBrushType = None;
	MAP_BLENDTYPE mBlendType = BLEND1;
	//=============================//


	//=======씬타입&Export타입======//
	vector<wstring> ObjectNames;
	wstring selectObjName;
	//=============================//


	//==================지형관련===================//
	map<wstring, class Terrain*> terrains;
	map<wstring, class CubeSky*> cubeSkys;
	map<wstring, class Sky*> skys;
	map<wstring, class Rain*> rains;

	Effect*lineEffect;
	//==========================================//


	//========================에셋창===========================//
	vector<class Terrain*> loadTerrains;			//머터리얼파일들
	vector<class Sky*> loadSkys;		//모델파일들
	vector<class Rain *> loadRains;		//에니메이션파일들

	char savePath[10] = { 0, };				//저장경로

											//에셋에서 하이라키에 보낼 이름정보들//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
