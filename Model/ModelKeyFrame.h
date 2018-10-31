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
	//시간에 해당하는 프레임인덱스 반환
	UINT GetKeyframeIndex(float time);

	//시작구간 인덱스와 그다음구간 인덱스를 시간에 맞춰서 변환
	void CalcKeyframeIndex(float time,
		bool bRepeat,
		OUT UINT& index1,
		OUT UINT& index2,
		OUT float& interpolatedTime);

	//두개의 인덱스를 가지고 보간
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
