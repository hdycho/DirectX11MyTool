#pragma once
#include "Systems/IExecute.h"

class SceneManager
{
public:
	static void Create();
	static void Delete();
	static SceneManager*Get() { return instance; }

private:
	static SceneManager*instance;

public:
	enum class SCENETYPE
	{
		StartScene,
		LoadingScene,
		LoadingScene2,
		Stage1,
		EndScene
	}sType;
public:
	void InputScene(SCENETYPE sceneName, class IExecute*scene);

	void Init();
	void Update();
	void Ready();

	void PreRender();
	void Render();
	void PostRender();

	void Destroy();

	void ChangeScene(SCENETYPE changeSceneName);
	void StartLoadingScene();

	IExecute*GetCurScene() { return scenes[(int)sType]; }

	class ProgressBar*GerProgress() { return pbar; }
private:
	void ParticleInit();
	void UIInit();
	void SoundInit();
private:
	vector<class IExecute*> scenes;

	bool bInitialize;
	float progress;
	mutex*mu;

	thread t;
	class ProgressBar*pbar;
};
