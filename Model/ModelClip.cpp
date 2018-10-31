#include "stdafx.h"
#include "ModelClip.h"
#include "Model.h"
#include "../Utilities/BinaryFile.h"
#include "ModelKeyFrame.h"

ModelClip::ModelClip(wstring file)
	: playTime(0.0f)
	, bRepeat(false), speed(1.0f), bLockRoot(false)
{
	playState = false;

	curFrame = 0;
	fileName = Path::GetFileName(file);
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	//모델클립의 데이터 읽어오기
	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();

	keyframesCount = r->UInt(); // 키프레임의 갯수
	//키프레임갯수만큼 반복
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyFrame* keyframe = new ModelKeyFrame();
		keyframe->boneName = String::ToWString(r->String());
		keyframe->index = r->UInt();
		keyframe->parent = r->UInt();

		keyframe->duration = duration;
		keyframe->frameCount = frameCount;
		keyframe->frameRate = frameRate;

		size = r->UInt();
		if (size > 0)
		{
			keyframe->transforms.assign(size, ModelKeyFrame::Transform());

			//키프레임배열 복사
			void* ptr = (void *)&keyframe->transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyFrame::Transform) * size);
		}
		//키프레임들을 맵으로 관리 -> 본의 이름, 키프레임
		keyframeMap.insert(Pair(keyframe->boneName, keyframe));
		boneNames.push_back(keyframe->boneName);
	}

	r->Close();
	BindingKeyFrame();

	SAFE_DELETE(r);
}

ModelClip::ModelClip(ModelClip & copy)
{
	playTime = copy.playTime;
	bRepeat = copy.bRepeat;
	speed = copy.speed;
	bLockRoot = copy.bLockRoot;

	playState = copy.playState;
	curFrame = copy.curFrame;
	fileName = copy.fileName;
	name = copy.name;

	duration = copy.duration;
	frameRate = copy.frameRate;
	frameCount = copy.frameCount;

	keyframesCount = copy.keyframesCount;
	size = copy.size;

	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyFrame* keyframe = new ModelKeyFrame();
		keyframe->boneName = copy.keyframeMap[copy.boneNames[i]]->boneName;
		keyframe->index = copy.keyframeMap[copy.boneNames[i]]->index;
		keyframe->parent = copy.keyframeMap[copy.boneNames[i]]->parent;

		keyframe->duration = copy.keyframeMap[copy.boneNames[i]]->duration;
		keyframe->frameCount = copy.keyframeMap[copy.boneNames[i]]->frameCount;
		keyframe->frameRate = copy.keyframeMap[copy.boneNames[i]]->frameRate;

		if (size > 0)
		{
			keyframe->transforms.assign(
				copy.keyframeMap[copy.boneNames[i]]->transforms.begin(),
				copy.keyframeMap[copy.boneNames[i]]->transforms.end());
		}
		//키프레임들을 맵으로 관리 -> 본의 이름, 키프레임
		keyframeMap.insert(Pair(keyframe->boneName, keyframe));
	}

	BindingKeyFrame();
}

ModelClip::~ModelClip()
{
	for (Pair keyframe : keyframeMap)
		SAFE_DELETE(keyframe.second);
}

void ModelClip::Reset()
{
	bRepeat = false;
	speed = 1.0f;
	playTime = 0.0f;
}

//모델본과 경과시간을 보내서 해당본의 변환된 행렬반환
D3DXMATRIX ModelClip::GetKeyframeMatrix(ModelBone * bone, float time)
{
	wstring boneName = bone->Name();
	unordered_map<wstring, ModelKeyFrame *>::iterator it;
	
	//키프레임맵에서 해당본이름 못찾으면 그냥 버린다
	if ((it = keyframeMap.find(boneName)) == keyframeMap.end())
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}


	//본의 이름으로 키프레임맵에서 키프레임정보 하나 빼옴
	ModelKeyFrame* keyframe = keyframeMap.at(boneName);

	playTime += speed * time; // 에니메이션 속도
	curFrame = (playTime / duration)*(float)frameCount;

	if (bRepeat == true)
	{
		//반복상황일때 경과시간이 전체시간 넘으면
		//경과시간에 전체시간을 한번빼준다(경과시간0으로만들기)
		if (playTime >= duration)
		{
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		//한번재생일때는 전체시간넘으면 경과시간 전체시간으로 고정한다
		if (playTime >= duration)
		{
			playTime = duration;
			playState = false;
		}
	}
	//본의 글로벌의 역행렬은 로컬월드가 나온다
	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	//해당 키프레임에서 경과시간에 따른 변환행렬 반환받는다
	D3DXMATRIX animation = keyframe->GetInterpolatedMatrix(playTime, bRepeat);


	//해당본의 부모행렬 구한다
	
	D3DXMATRIX parent;
	int parentIndex = bone->ParentIndex();
	if (parentIndex < 0)
	{
		//부모가 루트노드일때 루트를 고정시킬때
		//루트노드는 0,0,0 이므로
		if (bLockRoot == true)
			D3DXMatrixIdentity(&parent);
		else
			parent = animation;
	}
	else
	{
		if(bone->IsChangeLocal())
			parent = bone->GetChangeLocal()*animation * bone->Parent()->Global();
		else
			parent= animation * bone->Parent()->Global();
	}

	//해당 본의 로컬*해당 본의 부모의 월드 => 본의 월드

	return invGlobal * parent;
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);

	bone->Local(animation);
}

void ModelClip::ChangeBoneName(Model * model)
{
	//여기 복사잘되는지 확인해보기 이름은 잘들어옴
	int i = 0;
	unordered_map<wstring, ModelKeyFrame*> copyMap;
	unordered_map<wstring, ModelKeyFrame *>::iterator miter;

	//재귀로 처리해야하는 부분
	miter = keyframeMap.begin();
	InputAnimName(model, miter->second,model->BoneByIndex(0)->Index());

	for (; miter != keyframeMap.end(); miter++)
	{
		copyMap.insert(Pair(KeyframeByIndex(i)->BoneName(), KeyframeByIndex(i)));
		i++;
	}

	/*miter = keyframeMap.begin();
	for (; miter != keyframeMap.end(); miter++)
	{
		SAFE_DELETE(miter->second);
	}*/
	keyframeMap.clear();
	
	////세컨드복사가 재대로안됨
	miter = copyMap.begin();
	for (; miter != copyMap.end(); miter++)
	{
		keyframeMap.insert(Pair(miter->first, miter->second));
	}
}

void ModelClip::InputAnimName(Model * model, ModelKeyFrame * frame,int idx)
{
	if (model->BoneByIndex(idx)==NULL)return;

	int index = 0;
	index = idx;
	if (idx == 0)
	{
		if (model->BoneByIndex(idx)->ChildCount() == 0)
		{
			idx = 1;
			index = idx - 1;
		}
		else
			idx = 0;
	}

	KeyframeByIndex(index)->BoneName() = model->BoneByIndex(idx)->Name();

	for (int i = 0; i < model->BoneByIndex(idx)->GetChilds().size(); i++)
	{
		InputAnimName(model,KeyframeByIndex(frame->Childs[index]->index), model->BoneByIndex(idx)->Child(i)->Index());
	}
}

ModelKeyFrame * ModelClip::KeyframeByIndex(int idx)
{
	unordered_map<wstring, ModelKeyFrame *>::iterator miter;

	miter = keyframeMap.begin();
	for (;miter != keyframeMap.end(); miter++)
	{
		if (miter->second->index == idx)
		{
			return miter->second;
		}
	}
}

void ModelClip::BindingKeyFrame()
{
	unordered_map<wstring, ModelKeyFrame *>::iterator miter;

	for (int rootIdx = 0; rootIdx < keyframeMap.size(); rootIdx++)
	{
		miter = keyframeMap.begin();
		for (; miter != keyframeMap.end(); miter++) // 부모부터 시작해서 자식정보얻기
		{
			if (miter->second->parent == rootIdx)
			{
				KeyframeByIndex(rootIdx)->Childs.push_back(miter->second);
			}
		}
	}
}
