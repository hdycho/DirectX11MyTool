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
	//��������
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
	void PickedInit();			//��ŷ����false��
								//void PickSetting(bool value);	//��ŷ����Ű��

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
	//=============����==============//
	Effect*brushEffect;
	ID3D11ShaderResourceView*srv;
	ID3D11UnorderedAccessView*uav;

	wstring selectType;
	vector<wstring> typeNames;

	Texture*layerImg[3];
	//==============================//

	//==========EnumŸ��============//
	MAP_SETTYPE mSetType = SET_NONE;
	MAP_BRUSH mBrushType = None;
	MAP_BLENDTYPE mBlendType = BLEND1;
	//=============================//


	//=======��Ÿ��&ExportŸ��======//
	vector<wstring> ObjectNames;
	wstring selectObjName;
	//=============================//


	//==================��������===================//
	map<wstring, class Terrain*> terrains;
	map<wstring, class CubeSky*> cubeSkys;
	map<wstring, class Sky*> skys;
	map<wstring, class Rain*> rains;

	Effect*lineEffect;
	//==========================================//


	//========================����â===========================//
	vector<class Terrain*> loadTerrains;			//���͸������ϵ�
	vector<class Sky*> loadSkys;		//�����ϵ�
	vector<class Rain *> loadRains;		//���ϸ��̼����ϵ�

	char savePath[10] = { 0, };				//������

											//���¿��� ���̶�Ű�� ���� �̸�������//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
