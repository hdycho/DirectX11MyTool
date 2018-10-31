#pragma once
#include "GameModel.h"

class GameAnimModel : public GameModel
{
public:
	GameAnimModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile,
		ExecuteValues*values
	);

	GameAnimModel(GameAnimModel&gameModel);
	virtual ~GameAnimModel();

	virtual void Update();
	virtual void FixedUpdate() {};
	virtual void FixedInit() {};
	virtual void Render();

	virtual bool OnCollisionEnter() 
	{
		return false; 
	}
	virtual bool OnCollisionExit() 
	{ 
		return false; 
	}

	UINT AddClip(wstring file,float startTime=0.0f);

	void Repeat(UINT index, bool val);
	void Speed(UINT index, float val);
	void LockRoot(UINT index, bool val);

	void Play(UINT index,
		bool bRepeat = false,
		float blendTime = 0,
		float speed = 1.0f,
		float startTime = 0.0f);

	void Play(UINT index,
		float blendTime,
		bool bRepeat,
		float startTime);

	void Reset(UINT index);

	void Stop();

	vector<class ModelClip *>&GetAnimClip() { return clips; }
	class ModelTweener*&GetTweener() { return tweener; }
private:
	vector<class ModelClip *> clips;
	class ModelTweener*tweener;

	bool isStop;
};