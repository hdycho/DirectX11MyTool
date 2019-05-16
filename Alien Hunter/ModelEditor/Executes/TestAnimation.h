#pragma once
#include "Systems/IExecute.h"

class TestAnimation : public IExecute
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

	void TestImgui();

private:
	GameAnimator*model[3];
	ModelClip*clip;
	AnimInstance*animInstance;
};