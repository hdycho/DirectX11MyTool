#pragma once
#include "stdafx.h"

class GameTool
{
protected:
	enum MENUTYPE
	{
		MENU_NONE,
		EXPORT_DATA,
		LOAD_MODEL,
		LOAD_ANIMATION,
		LOAD_PLAYER,
		LOAD_ENEMY1,
		LOAD_BOSS,
		LOAD_DATA, // ������ ���Ϻҷ�����
		LOAD_SCENE,
		SAVE_DATA  // ������繰ü����
	};
	enum SCENETYPE
	{
		EDITSCENE,
		GAMESCENE
	};
	struct ExportType
	{
		//���̶�Ű���� �������¼���
		bool isShowCombLandScape;

		
		bool isShowCombCharactor;
		bool isShowCombModel;
		bool isShowObject;
		bool isColliderOpen;
		bool isTrailOpen;
		bool isLoadMesh;

		bool isCollisionManagerOpen;


		ExportType()
		{
			isShowCombLandScape = false;
			isShowObject = false;
			isShowCombCharactor = false;
			isTrailOpen = false;
			isShowCombModel = false;
			isColliderOpen = false;
			isLoadMesh =false;
			isCollisionManagerOpen = false;
		}
	};

public:
	GameTool() {}
	~GameTool() {}

	virtual void Ready() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;
	virtual void Update() = 0;

protected:
	virtual void DrawHiarachy() = 0;
	virtual void DrawAsset() = 0;
	virtual void DrawInspector() = 0;

protected:
	ExportType eType;
	SCENETYPE sType = SCENETYPE::EDITSCENE;
	MENUTYPE mType = MENUTYPE::MENU_NONE;

	class Gizmo*gizmo;
	class GizmoGrid*grid;
	class Shadow*shadow;
	bool isShowGrid = true;
};