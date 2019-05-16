#pragma once

struct ModelKeyFrameData
{
	float Time;
	D3DXVECTOR3 Scale;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
};

struct ModelKeyFrame
{
	wstring BoneName;
	vector<ModelKeyFrameData> Transforms;
};

class ModelClip
{
public:
	ModelClip(wstring file, bool isRepeat = false, bool isInPlace = false);
	ModelClip(ModelClip&copy);
	~ModelClip();

	wstring Name() { return name; }
	wstring AnimName() { return animName; }
	wstring FileName() { return fileName; }
	float Duration() { return duration; }
	float FrameRate() { return frameRate; }
	UINT FrameCount() { return frameCount; }
	bool&IsRepeat() { return isRepeat; }
	float&Speed() { return speed; }
	int&StartFrame() { return startFrame; }
	int&EndFrame() { return endFrame; }
	float&BlendTime() { return blendTime; }
	bool&IsAnimEnd() { return isAnimEnd; }
	bool&IsInPlace() { return isInPlace; }
	UINT&ClipIndex() { return clipIndex; }

	ModelKeyFrame*Keyframe(wstring name);
	ModelKeyFrame*Keyframe(UINT idx);
private:
	wstring name;
	wstring animName;
	wstring fileName;

	float duration;
	float frameRate;
	float blendTime;
	UINT frameCount;

	bool isRepeat;
	float speed;
	int startFrame;
	int endFrame;
	bool isAnimEnd;

	bool isInPlace;
	UINT clipIndex; // 클립인덱스 몇번인지

	unordered_map<wstring, ModelKeyFrame *> keyframeMap;

	vector<ModelKeyFrame*> keyframeVector;
};