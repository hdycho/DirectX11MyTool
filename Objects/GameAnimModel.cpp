#include "stdafx.h"
#include "GameAnimModel.h"
#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"

GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, 
	wstring meshFolder, wstring meshFile, ExecuteValues*values)
	:GameModel(matFolder, matFile, meshFolder, meshFile,values)
{
	model->Buffer()->UseBlend(true);

	myPtr = this;
	modelType = L"DefaultAnim";
	className= L"DefaultAnim";

	tweener = new ModelTweener();
	isStop = false;
}

GameAnimModel::GameAnimModel(GameAnimModel & gameModel)
	:GameModel(gameModel)
{
	model->Buffer()->UseBlend(true);
	for (int i = 0; i < gameModel.clips.size(); i++)
	{
		ModelClip*clip = new ModelClip(*gameModel.clips[i]);
		clips.push_back(clip);
	}

	myPtr = this;
	modelType = L"DefaultAnim";
	tweener = new ModelTweener();
	*tweener = *gameModel.GetTweener();
	isStop = false;
}

GameAnimModel::~GameAnimModel()
{
	for (ModelClip*clip : clips)
		SAFE_DELETE(clip);

	SAFE_DELETE(tweener);
}

void GameAnimModel::Update()
{
	if (clips.size() > 0)
	{
		CalcPosition();
		for (UINT i = 0; i < model->BoneCount(); i++)
		{
			ModelBone*bone = model->BoneByIndex(i);
			//모델의 본에 직접 접근하여 로컬값들을 바꿔준다
			if(!isStop)
				tweener->UpdateBlending(bone, Time::Delta());
			else
				tweener->UpdateBlending(bone, 0);
		}
		//그다음 변한 본의 로컬을가지고 월드를 재계산한다.
	}
	__super::Update();
}

void GameAnimModel::Render()
{
	__super::Render();
	normalBuffer->SetPSBuffer(5); //=> 이거 게임에님모델로보내자
}

UINT GameAnimModel::AddClip(wstring file, float startTime)
{
	ModelClip*clip = new ModelClip(file);
	clips.push_back(clip);
	return 0;
}

void GameAnimModel::Repeat(UINT index, bool val)
{
	clips[index]->Repeat(val);
}

void GameAnimModel::Speed(UINT index, float val)
{
	clips[index]->Speed(val);
}

void GameAnimModel::LockRoot(UINT index, bool val)
{
	clips[index]->LockRoot(val);
}

void GameAnimModel::Play(UINT index, bool bRepeat, float blendTime, float speed, float startTime)
{
	isStop = false;
	clips[index]->PlayState() = true;
	tweener->Play(clips[index], bRepeat, blendTime, speed, startTime);
}

void GameAnimModel::Play(UINT index, float blendTime, bool bRepeat, float startTime)
{
	isStop = false;
	tweener->Play(clips[index], bRepeat,blendTime,startTime);
}

void GameAnimModel::Reset(UINT index)
{
	clips[index]->Reset();
	for (int i = 0; i < model->Bones().size(); i++)
	{
		model->Bones()[i]->Local()=initBonesTransforms[i];
	}
}

void GameAnimModel::Stop()
{
	isStop = true;
}
