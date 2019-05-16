#pragma once

class GameAnimator :public GameModel
{
public:
	enum class Mode
	{
		Play = 0,
		Pause,
		Stop
	};

public:
	GameAnimator(Effect*lineEffect = NULL);
	GameAnimator(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect = NULL);
	GameAnimator(GameAnimator&copy);
	virtual ~GameAnimator();

	void AddClip(wstring clipFile, bool isRepeat = false, bool isInplace = true);
	virtual void Ready();
	virtual void Update();

	void StartTargetClip(wstring clipName);
	class ModelClip*GetPlayClip();
	class ModelClip*GetClip(wstring name) { return clips[name]; }
	queue<class ModelClip*>&GetClipQueue() { return clipQueue; }

	wstring CurClipName() { return currentClipName; }
	UINT&CurrentKeyFrame() { return currentKeyframe; }

	void Play() { mode = Mode::Play; }
	void Pause() { mode = Mode::Pause; }
	void Stop() { mode = Mode::Stop; }

	map<wstring, class ModelClip*>&AnimClips() { return clips; }

	D3DXMATRIX GetBoneMatrix(int idx) override;
	D3DXMATRIX GetBoneMatrixOutRotate(int idx);
	virtual D3DXMATRIX GetPlayerBoneWorld(int idx);

	void InplaceClipUpdate(bool start);

	virtual class GData* Save();
	virtual void Load(wstring fileName);

	virtual void ColliderUpdate() {}

protected:
	virtual void UpdateWorld();

private:
	void AnimationTransforms();
	void BlendTransforms();

	void UpdateTransforms();

private:
	D3DXMATRIX*InboneMatrix; //외부에서 들어오는본메트릭스
	D3DXMATRIX*InitMatrix;
	D3DXMATRIX*boneTransforms;
	D3DXMATRIX*renderTransforms;
public:
	void SetBoneMatrix(D3DXMATRIX mat, int index);
	virtual bool&LoadStart() { return isLoad; }

private:
	Mode mode;

	wstring currentClipName;
	UINT currentKeyframe;
	UINT nextKeyframe;

	float frameTime;
	float time;
	float blendStartTime;

	map<wstring, class ModelClip*> clips;
	bool isInit; // 원래위치로돌리는 것

	queue<class ModelClip*> clipQueue;

	bool isBlending;
	UINT clipIndex = 1;

	bool changeInplace;
	D3DXVECTOR3 inplacePos = { 0,0,0 };
protected:
	bool isFirstModelName;
private:
	//bool useBoneTransforms = false;
	bool useBoneTransforms = true; // 플레이어만쓴다
protected:
	void UseBoneTransforms() { useBoneTransforms = true; }
};