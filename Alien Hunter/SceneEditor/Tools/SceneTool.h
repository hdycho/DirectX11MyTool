#pragma once
#include "ToolHeader\GameTool.h"

class SceneTool :public GameTool
{
private:
	struct AssetLoadType
	{
		bool showTerrain;
		bool showSky;
		bool showRain;
		bool showMesh;
		bool showModel;
		bool showCharactor;

		AssetLoadType()
		{
			showTerrain = false;
			showSky = false;
			showRain = false;
			showMesh = false;
			showModel = false;
			showCharactor = false;
		}
	}aType;

private:
	//오브젝트관리
	typedef map<wstring, class Terrain*>::iterator terrainIter;
	typedef map<wstring, class Sky*>::iterator SkyIter;
	typedef map<wstring, class Rain*>::iterator rainIter;
	typedef map<wstring, GameModel*>::iterator modelIter;
	typedef map<wstring, GameAnimator*>::iterator animIter;
public:
	SceneTool();
	~SceneTool();

	void Ready() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Update() override;

private:
	void PickedInit();			//피킹전부false로
								//void PickSetting(bool value);	//피킹끄고키기

	void UseDebugLine();

	void DrawAsset() override;
	void DrawInspector() override;
	void DrawHiarachy() override;

	wstring CutNumberString(wstring name);

	void CheckSelectObject();
	void CharactorSetRender(GameAnimator*anim);

	void LoadModelData(wstring fileName);
	void LoadScene(wstring sceneName);

private:
	

	//=======씬타입&Export타입======//
	vector<wstring> ObjectNames;
	map<wstring, wstring> modelGroupNames;
	map<wstring, wstring> animGroupNames;
	wstring selectObjName;
	bool useGizmo = true;
	//=============================//


	//==================오브젝트관련===================//
	Effect*lineEffect;

	map<wstring, class Terrain*> terrains;
	map<wstring, class CubeSky*> cubeSkys;
	map<wstring, class Sky*> skys;
	map<wstring, class Rain*> rains;
	map<wstring, GameModel*> models;
	map<wstring, GameAnimator*> animModels;
	//==============================================//


	//========================에셋창===========================//
	vector<class Terrain*> loadTerrains;			//머터리얼파일들
	vector<class Sky*> loadSkys;					//모델파일들
	vector<class Rain *> loadRains;					//에니메이션파일들
	vector<GameModel*> loadModel;					//모델파일들
	vector<GameAnimator*> loadCharactor;	//케릭터파일들

	map<wstring, UINT> modelIDs;

	char savePath[10] = { 0, };			//저장경로

										//에셋에서 하이라키에 보낼 이름정보들//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
