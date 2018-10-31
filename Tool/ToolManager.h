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

		//메니저설정
		bool isShowCollision;
		bool isShowShadow;
		bool isShowCamera;

		//하이라키에서 보여지는설정
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
	//툴관리
	typedef map<TOOLTYPE, GameTool*>::iterator toolIter;

	//모델관리
	typedef map<wstring, map<wstring,class GameModel*>> GameObject;
	typedef map<wstring, map<wstring,class GameModel*>>::iterator objIter;
	typedef map<wstring, class GameModel*>::iterator modelIter;
	
	//지형관리
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
	void ExportFile(wstring filename); // 파일추출
	void LoadFile(wstring filename);   // 파일로드
	void LoadModelMapFile(class GameModel*model,wstring filename, int type, int idx=-1);
	void LoadTerrainMapFile(class TerrainRender*tree, wstring filename, int type);
	void ShowModelBone(wstring name, int index,wstring modelType);

	//이거수정필요
	void PickedInit();				//피킹전부false로
	void PickSetting(bool value);	//피킹끄고키기

	void DrawAsset();
	void DrawInspector();
	void DrawHiarachy();

	//게임모델에 상속된 새로운 클래스설정 여기서
	void SettingClassTag(Shader*shader,wstring folderName,wstring matName,wstring fileName);
private:
	ExecuteValues*values;
	class Frustum*frustum;

	//=========툴종류관련===========//
	map<TOOLTYPE, GameTool*> tools;
	TOOLTYPE curToolType;
	wstring selectToolName;
	//=============================//


	//=======씬타입&Export타입======//
	SCENETYPE sType;
	MENUTYPE mType;
	bool isOpen; // 게임씬이랑 에딧씬에서 상태창보이게하는것
	//=============================//


	//==================오브젝트===================//
	GameObject objects;
	Shader*CharactorShader;

	//임구이 콤보에 들어가서 업데이트하는값들//
	map<wstring, vector<wstring>> animNames; 
	wstring selectAnimName;	   

	vector<wstring> modelNames;
	wstring selectmodelName;

	vector<wstring> tagNames;
	wstring selectTagName;
	//============================================//


	//==================지형관련===================//
	map<wstring, class TerrainRender*> terrains;
	map<wstring, class Water*> waters;



	//==========================================//

	//========================에셋창===========================//
	ExportType eType;
	vector<class Material*> matVec;			//머터리얼파일들
	vector<class GameModel*> modelVec;		//모델파일들
	vector<class ModelClip *> animVec;		//에니메이션파일들

	char savePath[10] = { 0, };				//저장경로
	char className[20] = { 0, };			//클래스타입

	//에셋에서 하이라키에 보낼 이름정보들//
	wstring dragMatName;
	D3DXVECTOR3 dragMatColor;

	wstring dragModelName;
	D3DXVECTOR3 dragModelColor;

	wstring dragAnimName;
	D3DXVECTOR3 dragAnimColor;
	//========================================================//
};
