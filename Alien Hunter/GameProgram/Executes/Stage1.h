#pragma once
#include "Systems/IExecute.h"

class Stage1 : public IExecute
{
public:
	enum EventState
	{
		PRESTAGE,
		STAGE1,
		PRESTAGE2,
		STAGE2,
		PREENDING,
		ENDING,
		FAIL
	}eState;
public:
	void Initialize() override;
	void Ready() override;
	void Destroy() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override;

private:
	void NpcUpdate();
	void StageUpdate();
	void EnemyTargetUpdate();

private:
	struct ModelUI
	{
		NearEnemy*enemy;
		float x;
		float y;
		float scaleX;
		float scaleY;
		float hpX;
		float hpY;
		float hpScaleX;
		float hpScaleY;
	};

private:
	class Camera*mainCam;
	class TPSCamera*tpsCam;
	class GameAnimator*npc;
	class GameModel*ship;

	int preStageCount = 0;
	float clearStageTime;
	float reStageTime;
	D3DXVECTOR3 gateOpos;
	bool isStageStart = false;
	
	bool uiInit;
	bool onceFade;

	ModelUI preUi;
};