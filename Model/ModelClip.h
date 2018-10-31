#pragma once
#include "ModelKeyFrame.h"

class ModelClip
{
public:
	ModelClip(wstring file);
	ModelClip(ModelClip&copy);
	~ModelClip();

	void Reset();

	D3DXMATRIX GetKeyframeMatrix(class ModelBone* bone, float time);
	void UpdateKeyframe(class ModelBone* bone, float time);

	void LockRoot(bool val) { bLockRoot = val; }
	void Repeat(bool val) { bRepeat = val; }
	void Speed(float val) { speed = val; }

	void StartTime(float val) { playTime = val; }
	float&StartTime() { return playTime; }
	float&Duration() { return duration; }
	int FrameCount() { return (int)frameCount; }
	int CurrentFrame() { return curFrame; }
	float&GetSpeed() { return speed; }
	
	wstring GetName() { return fileName; }
	void ChangeBoneName(class Model*model);
	void InputAnimName(class Model*model, class ModelKeyFrame* frame,int idx);

	ModelKeyFrame*KeyframeByIndex(int idx);

	void BindingKeyFrame();

	bool&PlayState() { return playState; }
private:
	bool playState;

	wstring name;
	wstring fileName;

	UINT keyframesCount;
	UINT size;

	vector<wstring> boneNames; // 복사를위한배열

	float duration;		//	에니메이션 총시간
	float frameRate;	//	에니메이션 속도
	UINT frameCount;	//	에니메이션 프레임수
	UINT curFrame;

	float checkTime;

	bool bLockRoot;		//	부모를 고정시킬지
	bool bRepeat;		//	반복시킬지
	float speed;		//	속도
	float playTime;		//	경과시간

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyFrame *> Pair;
	//뼈마다 가지고있는 프레임수
	unordered_map<wstring, ModelKeyFrame *> keyframeMap;
};