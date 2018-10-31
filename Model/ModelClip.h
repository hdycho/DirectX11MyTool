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

	vector<wstring> boneNames; // ���縦���ѹ迭

	float duration;		//	���ϸ��̼� �ѽð�
	float frameRate;	//	���ϸ��̼� �ӵ�
	UINT frameCount;	//	���ϸ��̼� �����Ӽ�
	UINT curFrame;

	float checkTime;

	bool bLockRoot;		//	�θ� ������ų��
	bool bRepeat;		//	�ݺ���ų��
	float speed;		//	�ӵ�
	float playTime;		//	����ð�

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyFrame *> Pair;
	//������ �������ִ� �����Ӽ�
	unordered_map<wstring, ModelKeyFrame *> keyframeMap;
};