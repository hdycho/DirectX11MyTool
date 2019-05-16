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
	//������Ʈ����
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
	void PickedInit();			//��ŷ����false��
								//void PickSetting(bool value);	//��ŷ����Ű��

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
	

	//=======��Ÿ��&ExportŸ��======//
	vector<wstring> ObjectNames;
	map<wstring, wstring> modelGroupNames;
	map<wstring, wstring> animGroupNames;
	wstring selectObjName;
	bool useGizmo = true;
	//=============================//


	//==================������Ʈ����===================//
	Effect*lineEffect;

	map<wstring, class Terrain*> terrains;
	map<wstring, class CubeSky*> cubeSkys;
	map<wstring, class Sky*> skys;
	map<wstring, class Rain*> rains;
	map<wstring, GameModel*> models;
	map<wstring, GameAnimator*> animModels;
	//==============================================//


	//========================����â===========================//
	vector<class Terrain*> loadTerrains;			//���͸������ϵ�
	vector<class Sky*> loadSkys;					//�����ϵ�
	vector<class Rain *> loadRains;					//���ϸ��̼����ϵ�
	vector<GameModel*> loadModel;					//�����ϵ�
	vector<GameAnimator*> loadCharactor;	//�ɸ������ϵ�

	map<wstring, UINT> modelIDs;

	char savePath[10] = { 0, };			//������

										//���¿��� ���̶�Ű�� ���� �̸�������//
	wstring selectTagName;
	vector<wstring> tagNames;

	wstring selectLoadDataName;
	D3DXVECTOR3 loadDataColor;
	//========================================================//
};
