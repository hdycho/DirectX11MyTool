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

	//����ؼ� ���������� ���ϰ� ���������� ���Ѵ�
	CalcKeyframeIndex(time, bRepeat, index1, index2, interpolatedTime);

	//���������� �ش��ϴ� ������ ���������� ���� ��ĺ�ȯ�� ��ȯ
	return GetInterpolatedMatrix(index1, index2, interpolatedTime);
}

UINT ModelKeyFrame::GetKeyframeIndex(float time)
{
	UINT start = 0, end = frameCount - 1;

	if (time >= transforms[end].Time)
		return end; // �÷��̰� ����� ���̱� ������ end�� ����

	do
	{
		// ���� �����ӹ�ȣ ����
		UINT middle = (start + end) / 2;

		if (end - start <= 1)
			break;
		else if (transforms[middle].Time < time)
		{
			//����ð��� �������� �߰��ð��� ������
			//�������� �߰����� ����
			start = middle;
		}
		else if (transforms[middle].Time > time)
		{
			//����ð��� �߰��������� �ð����� ������
			//���� �߰����� ����
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
	//index1 ���� �������� �ε���
	//index2 ���� �������� �ε���

	index1 = index2 = 0;
	interpolatedTime = 0.0f;

	//����ð��� �´� �������ε��� 
	index1 = GetKeyframeIndex(time);

	//�ݺ���Ȳ�� �ƴҶ� ��ü�����Ӽ��� ���������� ���翡�� 1������ �ε����� �ε���2��
	//�����Ѵ�
	if (bRepeat == false)
		index2 = (index1 >= frameCount - 1) ? index1 : index1 + 1;
	else // �ݺ���Ȳ�϶��� ��ü�����Ӽ� ������ �ٽ� 0���� �����ش�
		index2 = (index1 >= frameCount - 1) ? 0 : index1 + 1;

	//��ü �����Ӽ� ������ �ݺ���Ȳ�� �ƴҶ� �ΰ��� ������ �����Ӽ��� �����ش�
	//�׸��� �����ð��� 1�� ����� 
	if (index1 >= frameCount - 1)
	{
		index1 = index2 = frameCount - 1;
		interpolatedTime = 1.0f;
	}
	else
	{
		//�����Ӱ� �����ӻ��̸� �ð������Ͽ� ������ ������ ���
		float time1 = time - transforms[index1].Time;
		float time2 = transforms[index2].Time - transforms[index1].Time;
		interpolatedTime = time1 / time2; // time1�� time2 ������ �ð� ������ ����.
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