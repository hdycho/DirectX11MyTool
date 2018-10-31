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

	//��Ŭ���� ������ �о����
	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();

	keyframesCount = r->UInt(); // Ű�������� ����
	//Ű�����Ӱ�����ŭ �ݺ�
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

			//Ű�����ӹ迭 ����
			void* ptr = (void *)&keyframe->transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyFrame::Transform) * size);
		}
		//Ű�����ӵ��� ������ ���� -> ���� �̸�, Ű������
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
		//Ű�����ӵ��� ������ ���� -> ���� �̸�, Ű������
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

//�𵨺��� ����ð��� ������ �ش纻�� ��ȯ�� ��Ĺ�ȯ
D3DXMATRIX ModelClip::GetKeyframeMatrix(ModelBone * bone, float time)
{
	wstring boneName = bone->Name();
	unordered_map<wstring, ModelKeyFrame *>::iterator it;
	
	//Ű�����Ӹʿ��� �ش纻�̸� ��ã���� �׳� ������
	if ((it = keyframeMap.find(boneName)) == keyframeMap.end())
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}


	//���� �̸����� Ű�����Ӹʿ��� Ű���������� �ϳ� ����
	ModelKeyFrame* keyframe = keyframeMap.at(boneName);

	playTime += speed * time; // ���ϸ��̼� �ӵ�
	curFrame = (playTime / duration)*(float)frameCount;

	if (bRepeat == true)
	{
		//�ݺ���Ȳ�϶� ����ð��� ��ü�ð� ������
		//����ð��� ��ü�ð��� �ѹ����ش�(����ð�0���θ����)
		if (playTime >= duration)
		{
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		//�ѹ�����϶��� ��ü�ð������� ����ð� ��ü�ð����� �����Ѵ�
		if (playTime >= duration)
		{
			playTime = duration;
			playState = false;
		}
	}
	//���� �۷ι��� ������� ���ÿ��尡 ���´�
	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	//�ش� Ű�����ӿ��� ����ð��� ���� ��ȯ��� ��ȯ�޴´�
	D3DXMATRIX animation = keyframe->GetInterpolatedMatrix(playTime, bRepeat);


	//�ش纻�� �θ���� ���Ѵ�
	
	D3DXMATRIX parent;
	int parentIndex = bone->ParentIndex();
	if (parentIndex < 0)
	{
		//�θ� ��Ʈ����϶� ��Ʈ�� ������ų��
		//��Ʈ���� 0,0,0 �̹Ƿ�
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

	//�ش� ���� ����*�ش� ���� �θ��� ���� => ���� ����

	return invGlobal * parent;
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);

	bone->Local(animation);
}

void ModelClip::ChangeBoneName(Model * model)
{
	//���� �����ߵǴ��� Ȯ���غ��� �̸��� �ߵ���
	int i = 0;
	unordered_map<wstring, ModelKeyFrame*> copyMap;
	unordered_map<wstring, ModelKeyFrame *>::iterator miter;

	//��ͷ� ó���ؾ��ϴ� �κ�
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
	
	////�����庹�簡 ���ξȵ�
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
		for (; miter != keyframeMap.end(); miter++) // �θ���� �����ؼ� �ڽ��������
		{
			if (miter->second->parent == rootIdx)
			{
				KeyframeByIndex(rootIdx)->Childs.push_back(miter->second);
			}
		}
	}
}
