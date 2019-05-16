#pragma once
#include "Systems/IExecute.h"

class LoadingScene2 : public IExecute
{
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
	class Camera*mainCam;
	class Follow*follow;

	class GameModel*ship;
	class GameModel*highway;
	class GameAnimator*player;

	D3DXVECTOR3 moveVec;

	int value;

	class CubeSky*sky;

	Render2D*dcImage;
	Texture*startImage;

	int size;
	bool isStart;
	float alpha;
	float startTime;
	float nextTime;
	float alphaValue;

	int nextStage;

	D3DXVECTOR2 curq;
	bool isNextStage;
	bool isNextStage2;
};