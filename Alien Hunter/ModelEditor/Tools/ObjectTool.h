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
	//������Ʈ����
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
	void PickedInit();			//��ŷ����false��
								//void PickSetting(bool value);	//��ŷ����Ű��

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
	//=============��==============//
	wstring animName;
	map<wstring, vector<BoneSphere*>> boneMeshes;
	//==============================//


	//==========EnumŸ��============//


	//=============================//


	//=======��Ÿ��&ExportŸ��======//
	vector<wstring> ObjectNames;
	wstring selectObjName;
	//=============================//


	//==================������Ʈ����===================//
	map<wstring, Mesh*> meshes;
	Material*meshMaterial;
	Effect*lineEffect;
	Effect*trailEffect;

	map<wstring, GameModel*> models;
	map<wstring, GameAnimator*> animModels;
	//==============================================//


	//==================�ݶ��̴��̸�====================//
	wstring selectColliderName;
	vector<wstring> colliderNames;
	//=================================================//


	//========================����â===========================//
	vector<Mesh*> loadMeshes;			//�޽����ϵ�
	vector<GameModel*> loadModel;		//�����ϵ�
	vector<GameAnimator*> loadCharactor;//�ɸ������ϵ�

	char savePath[10] = { 0, };			//������

										//���¿��� ���̶�Ű�� ���� �̸�������//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
