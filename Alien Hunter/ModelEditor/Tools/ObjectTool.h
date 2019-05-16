#pragma once
#include "ToolHeader\GameTool.h"

class ObjectTool :public GameTool
{
private:
	struct AssetLoadType
	{
		bool showMesh;
		bool showModel;
		bool showCharactor;

		AssetLoadType()
		{
			showMesh = false;
			showModel = false;
			showCharactor = false;
		}
	}aType;

	enum AnimModelType
	{
		Model,
		Animation
	}AnimType;

private:
	//오브젝트관리
	typedef map<wstring, Mesh*>::iterator meshIter;
	typedef map<wstring, GameModel*>::iterator modelIter;
	typedef map<wstring, GameAnimator*>::iterator animIter;
public:
	ObjectTool();
	~ObjectTool();

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
	void LoadMeshMapFile(Mesh*mesh, wstring file, int type);
	void LoadModelMapFile(Material*material, wstring file, int type);

	void ExportFile(wstring fileName);
	void LoadModel(wstring fileName);
	void LoadWeapon(Player*player, wstring fileName);
	void LoadCharactor(wstring fileName, MENUTYPE type);
	void LoadAnimation(GameAnimator*model, wstring fileName);

	void CharactorSetRender(GameAnimator*anim);

	void CreateBone(GameAnimator*model);
	void RenderBone();
	void UpdateBone(GameAnimator*model);
	void ShowBoneNode(GameAnimator*model, int index);
	void PostRenderBoneName();

	void LoadModelData(wstring fileName);

private:
	//=============툴==============//
	wstring animName;
	map<wstring, vector<BoneSphere*>> boneMeshes;
	//==============================//


	//==========Enum타입============//


	//=============================//


	//=======씬타입&Export타입======//
	vector<wstring> ObjectNames;
	wstring selectObjName;
	//=============================//


	//==================오브젝트관련===================//
	map<wstring, Mesh*> meshes;
	Material*meshMaterial;
	Effect*lineEffect;
	Effect*trailEffect;

	map<wstring, GameModel*> models;
	map<wstring, GameAnimator*> animModels;
	//==============================================//


	//==================콜라이더이름====================//
	wstring selectColliderName;
	vector<wstring> colliderNames;
	//=================================================//


	//========================에셋창===========================//
	vector<Mesh*> loadMeshes;			//메쉬파일들
	vector<GameModel*> loadModel;		//모델파일들
	vector<GameAnimator*> loadCharactor;//케릭터파일들

	char savePath[10] = { 0, };			//저장경로

										//에셋에서 하이라키에 보낼 이름정보들//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
