#include "Framework.h"
#include "ModelClip.h"

ModelClip::ModelClip(wstring file, bool isRepeat, bool isInPlace)
	:isRepeat(isRepeat), startFrame(0), speed(1), blendTime(6), isAnimEnd(false)
	, isInPlace(isInPlace)
{
	fileName = file;
	animName = L" " + Path::GetFileName(file);

	BinaryReader * r = new BinaryReader();
	r->Open(file);

	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();
	endFrame = frameCount - 2;

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyFrame * keyframe = new ModelKeyFrame();
		keyframe->BoneName = String::ToWString(r->String());

		r->UInt();
		r->UInt();

		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyFrameData());

			void * ptr = (void *)&keyframe->Transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyFrameData) * size);
		}

		keyframeMap[keyframe->BoneName] = keyframe;
		keyframeVector.push_back(keyframeMap[keyframe->BoneName]);
	}


	r->Close();
	SAFE_DELETE(r);
}

ModelClip::ModelClip(ModelClip & copy)
{
	isInPlace = copy.isInPlace;
	isRepeat = copy.isRepeat;
	startFrame = copy.startFrame;
	endFrame = copy.endFrame;
	speed = copy.speed;
	blendTime = copy.blendTime;
	isAnimEnd = copy.isAnimEnd;
	clipIndex = copy.clipIndex;

	name = copy.name;
	animName = copy.animName;
	fileName = copy.fileName;

	duration = copy.duration;
	frameRate = copy.frameRate;
	frameCount = copy.frameCount;

	keyframeMap.insert(copy.keyframeMap.begin(), copy.keyframeMap.end());

	keyframeVector.assign(copy.keyframeVector.begin(), copy.keyframeVector.end());
}

ModelClip::~ModelClip()
{
	unordered_map<wstring, ModelKeyFrame *>::iterator keyIter = keyframeMap.begin();

	for (; keyIter != keyframeMap.end(); keyIter++)
		SAFE_DELETE(keyIter->second);
}

ModelKeyFrame * ModelClip::Keyframe(wstring name)
{
	if (keyframeMap.count(name) < 1)
		return NULL;

	return keyframeMap[name];
}

ModelKeyFrame * ModelClip::Keyframe(UINT idx)
{
	if (keyframeVector.size() <= idx)
		return NULL;

	//TODO: 나중에 모델중에 루트본이 이상하게 되있는 애들이 있을거임
	//그거처리 별개로해야함
	return keyframeVector[idx];
}
