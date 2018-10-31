#pragma once

class ModelKeyFrame
{
public:
	struct Transform;
	friend class ModelClip;

	ModelKeyFrame();
	~ModelKeyFrame();

public:
	D3DXMATRIX GetInterpolatedMatrix(float time, bool bRepeat);
	wstring&BoneName() { return boneName; }
private:
	//�ð��� �ش��ϴ� �������ε��� ��ȯ
	UINT GetKeyframeIndex(float time);

	//���۱��� �ε����� �״������� �ε����� �ð��� ���缭 ��ȯ
	void CalcKeyframeIndex(float time,
		bool bRepeat,
		OUT UINT& index1,
		OUT UINT& index2,
		OUT float& interpolatedTime);

	//�ΰ��� �ε����� ������ ����
	D3DXMATRIX GetInterpolatedMatrix(UINT index1, UINT index2, float t);
private:
	wstring boneName;
	float duration;
	float frameRate;
	UINT frameCount;

	int index;
	int parent;

	vector<Transform> transforms;

	vector<ModelKeyFrame*> Childs;

public:
	struct Transform
	{
		float Time;
		D3DXVECTOR3 S;
		D3DXQUATERNION R;
		D3DXVECTOR3 T;
	};
};
