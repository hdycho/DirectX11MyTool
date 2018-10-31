#pragma once
#include "CharactorTool.h"
#include "MapTool.h"
#include "CameraTool.h"
#include <map>

class ToolManager
{
private:
	enum MENUTYPE
	{
		MENU_NONE,
		EXPORT_FBX,
		LOAD_MODEL,
		SAVE_MODEL
	};
	enum TOOLTYPE
	{
		NONE,
		OBJECTTOOL,
		TERRAINTOOL,
		CAMERATOOL
	};
	enum SCENETYPE
	{
		EDITSCENE,
		GAMESCENE
	};
	struct ExportType
	{
		bool isMaterial;
		bool isMesh;
		bool isAnim;
		bool showMaterial;
		bool showModel;
		bool showAnimation;

		//�޴�������
		bool isShowCollision;
		bool isShowShadow;
		bool isShowCamera;

		//���̶�Ű���� �������¼���
		bool isShowComboCharactor;
		bool isShowComboObject;
		bool isShowCombLandScape;

		ExportType()
		{
			isMaterial = isMesh = isAnim = false;
			showMaterial = showModel = showAnimation = true;
			isShowCollision = isShowShadow = isShowCamera=false;

			isShowComboCharactor = isShowComboObject = isShowCombLandScape = false;
		}
	};

private:
	//������
	typedef map<TOOLTYPE, GameTool*>::iterator toolIter;

	//�𵨰���
	typedef map<wstring, map<wstring,class GameModel*>> GameObject;
	typedef map<wstring, map<wstring,class GameModel*>>::iterator objIter;
	typedef map<wstring, class GameModel*>::iterator modelIter;
	
	//��������
	typedef map<wstring, class TerrainRender*>::iterator terrainIter;
	typedef map<wstring, class Water*>::iterator waterIter;
public:
	ToolManager(class ExecuteValues*values);
	~ToolManager();
	void Render();
	void Update();
	void PreRender();
	void PostRender();

	void Release();
private:
	void ExportFile(wstring filename); // ��������
	void LoadFile(wstring filename);   // ���Ϸε�
	void LoadModelMapFile(class GameModel*model,wstring filename, int type, int idx=-1);
	void LoadTerrainMapFile(class TerrainRender*tree, wstring filename, int type);
	void ShowModelBone(wstring name, int index,wstring modelType);

	//�̰ż����ʿ�
	void PickedInit();				//��ŷ����false��
	void PickSetting(bool value);	//��ŷ����Ű��

	void DrawAsset();
	void DrawInspector();
	void DrawHiarachy();

	//���Ӹ𵨿� ��ӵ� ���ο� Ŭ�������� ���⼭
	void SettingClassTag(Shader*shader,wstring folderName,wstring matName,wstring fileName);
private:
	ExecuteValues*values;
	class Frustum*frustum;

	//=========����������===========//
	map<TOOLTYPE, GameTool*> tools;
	TOOLTYPE curToolType;
	wstring selectToolName;
	//=============================//


	//=======��Ÿ��&ExportŸ��======//
	SCENETYPE sType;
	MENUTYPE mType;
	bool isOpen; // ���Ӿ��̶� ���������� ����â���̰��ϴ°�
	//=============================//


	//==================������Ʈ===================//
	GameObject objects;
	Shader*CharactorShader;

	//�ӱ��� �޺��� ���� ������Ʈ�ϴ°���//
	map<wstring, vector<wstring>> animNames; 
	wstring selectAnimName;	   

	vector<wstring> modelNames;
	wstring selectmodelName;

	vector<wstring> tagNames;
	wstring selectTagName;
	//============================================//


	//==================��������===================//
	map<wstring, class TerrainRender*> terrains;
	map<wstring, class Water*> waters;



	//==========================================//

	//========================����â===========================//
	ExportType eType;
	vector<class Material*> matVec;			//���͸������ϵ�
	vector<class GameModel*> modelVec;		//�����ϵ�
	vector<class ModelClip *> animVec;		//���ϸ��̼����ϵ�

	char savePath[10] = { 0, };				//������
	char className[20] = { 0, };			//Ŭ����Ÿ��

	//���¿��� ���̶�Ű�� ���� �̸�������//
	wstring dragMatName;
	D3DXVECTOR3 dragMatColor;

	wstring dragModelName;
	D3DXVECTOR3 dragModelColor;

	wstring dragAnimName;
	D3DXVECTOR3 dragAnimColor;
	//========================================================//
};
