#include "stdafx.h"
#include "ModelKeyFrame.h"

ModelKeyFrame::ModelKeyFrame()
{
}

ModelKeyFrame::~ModelKeyFrame()
{
}

D3DXMATRIX ModelKeyFrame::GetInterpolatedMatrix(float time, bool bRepeat)
{
	UINT index1 = 0, index2 = 0;
	float interpolatedTime = 0.0f;

	//계속해서 보간구간을 구하고 보간비율을 구한다
	CalcKeyframeIndex(time, bRepeat, index1, index2, interpolatedTime);

	//보간구간에 해당하는 비율과 보간구간을 통해 행렬변환식 반환
	return GetInterpolatedMatrix(index1, index2, interpolatedTime);
}

UINT ModelKeyFrame::GetKeyframeIndex(float time)
{
	UINT start = 0, end = frameCount - 1;

	if (time >= transforms[end].Time)
		return end; // 플레이가 종료된 것이기 때문에 end를 리턴

	do
	{
		// 절반 프레임번호 나옴
		UINT middle = (start + end) / 2;

		if (end - start <= 1)
			break;
		else if (transforms[middle].Time < time)
		{
			//진행시간이 프레임의 중간시간을 넘으면
			//시작점을 중간으로 보냄
			start = middle;
		}
		else if (transforms[middle].Time > time)
		{
			//진행시간이 중간프레임의 시간보다 작으면
			//끝을 중간으로 보냄
			end = middle;
		}
		else
		{
			start = middle;
			break;
		}

	} while ((end - start) > 1);

	return start;
}

void ModelKeyFrame::CalcKeyframeIndex(float time, bool bRepeat, OUT UINT & index1, 
	OUT UINT & index2, OUT float & interpolatedTime)
{
	//index1 현재 프레임의 인덱스
	//index2 다음 프레임의 인덱스

	index1 = index2 = 0;
	interpolatedTime = 0.0f;

	//현재시간에 맞는 프레임인덱스 
	index1 = GetKeyframeIndex(time);

	//반복상황이 아닐때 전체프레임수를 넘지않으면 현재에서 1을더한 인덱스를 인덱스2에
	//전달한다
	if (bRepeat == false)
		index2 = (index1 >= frameCount - 1) ? index1 : index1 + 1;
	else // 반복상황일때는 전체프레임수 넘으면 다시 0으로 보내준다
		index2 = (index1 >= frameCount - 1) ? 0 : index1 + 1;

	//전체 프레임수 넘으면 반복상황이 아닐때 두개다 마지막 프레임수로 맞춰준다
	//그리고 보간시간은 1로 맞춘다 
	if (index1 >= frameCount - 1)
	{
		index1 = index2 = frameCount - 1;
		interpolatedTime = 1.0f;
	}
	else
	{
		//프레임과 프레임사이를 시간을통하여 비율로 보간법 사용
		float time1 = time - transforms[index1].Time;
		float time2 = transforms[index2].Time - transforms[index1].Time;
		interpolatedTime = time1 / time2; // time1과 time2 사이의 시간 비율이 나옴.
	}
}

D3DXMATRIX ModelKeyFrame::GetInterpolatedMatrix(UINT index1, UINT index2, float t)
{
	D3DXVECTOR3 scale;
	D3DXMATRIX S;
	D3DXVec3Lerp(&scale, &transforms[index1].S, &transforms[index1].S, t);
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

	D3DXQUATERNION q;
	D3DXMATRIX R;
	D3DXQuaternionSlerp(&q, &transforms[index1].R, &transforms[index2].R, t);
	D3DXMatrixRotationQuaternion(&R, &q);

	D3DXVECTOR3 translation;
	D3DXMATRIX T;
	D3DXVec3Lerp(&translation, &transforms[index1].T, &transforms[index1].T, t);
	D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);

	return S * R * T;
}