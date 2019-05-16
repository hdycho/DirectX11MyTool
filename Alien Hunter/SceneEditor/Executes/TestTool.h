#pragma once
#include "Systems/IExecute.h"

class TestTool : public IExecute
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
	class SceneTool*tool;
	class ParticleSystem*ps;

	class GameModel*ship[50];
};