#pragma once
#include "Systems/IExecute.h"

class StartScene : public IExecute
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

	Render2D*dcImage;
	Texture*startImage;

	bool init;
	int size;
	bool isStart;
	float alpha;
};