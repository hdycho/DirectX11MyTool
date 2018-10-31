#include "stdafx.h"
#include "ModelTweener.h"
#include "ModelClip.h"

ModelTweener::ModelTweener()
	:current(NULL), next(NULL), blendTime(0.0f), elapsedTime(0.0f)
{
	isPlay = false;
}

ModelTweener::~ModelTweener()
{
}

void ModelTweener::Play(ModelClip * clip, bool bRepeat, float blendTime, float speed, float startTime)
{
	isPlay = true;
	this->blendTime = blendTime;
	this->elapsedTime = startTime;

	if (next == NULL)
	{
		next = clip;
		next->Repeat(bRepeat);
		next->Speed(speed);
		next->StartTime(startTime);
	}
	else
	{
		current = next;
		next = clip;
		next->Repeat(bRepeat);
		next->Speed(speed);
		next->StartTime(startTime);
	}
}

void ModelTweener::Play(ModelClip * clip, bool bRepeat, float blendTime, int startTime)
{
	isPlay = true;
	this->blendTime = blendTime;
	this->elapsedTime = startTime;

	if (next == NULL)
	{
		next = clip;
		next->Repeat(bRepeat);
		next->StartTime(startTime);
		//isPlay = false;
	}
	else
	{
		current = clip;
		current->Repeat(bRepeat);
		current->StartTime(startTime);
	}
}

void ModelTweener::UpdateBlending(ModelBone * bone, float time)
{
	if (isPlay)
	{
		D3DXMATRIX matrix;
		if (next != NULL)
		{
			elapsedTime += time;
			float t = elapsedTime / blendTime;

			//동작전환 완료
			if (t > 1.0f)
			{
				matrix = next->GetKeyframeMatrix(bone, time);
				current = next;
				next = NULL;

			}
			else
			{
				D3DXMATRIX start = current->GetKeyframeMatrix(bone, time);
				D3DXMATRIX end = next->GetKeyframeMatrix(bone, time);

				Math::LerpMatrix(matrix, start, end, t);
			}
		}
		else // 끝났다면
		{
			matrix = current->GetKeyframeMatrix(bone, time);
		}

		bone->Local(matrix);
	}
}
