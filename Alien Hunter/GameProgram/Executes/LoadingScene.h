#pragma once
#include "Systems/IExecute.h"

class LoadingScene : public IExecute
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

	TextureArray*textures;
	Render2D*render2d;

	int frameIdx;
};