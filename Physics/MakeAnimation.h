#pragma once

class MakeAnimation
{
public:
	enum ANIMSTATE
	{
		CONTROL,
		PLAY
	};
public:
	MakeAnimation(class GameModel*model,int boneIdx,D3DXVECTOR3*t, D3DXVECTOR3*s, D3DXVECTOR3*r);
	~MakeAnimation();

	void Update();
	void Render();

	void SaveAnimData();
	void LoadAnimData(wstring fName);

	void AnimPlay() { animState=PLAY; }
private:
	void UpdateTurret();

private:
	D3DXVECTOR3*trans, *scale, *rot;
	D3DXQUATERNION rotate;

	vector<D3DXVECTOR3> vTrans;
	vector<D3DXVECTOR3> vScale;
	vector<D3DXQUATERNION> vRotate;
	vector<int> vIndex;

	D3DXMATRIX boneMatrix;
	ModelBone*modelBone;

	vector<class ColliderBox*> partsColider;
	class ColliderBox*collider;

	int count;
	int index;
	int maxIndex;
	float time;
	float elapsedTime;
	float animSpeed;
	char speed[3] = { 0, };

	wstring modelName;

	ANIMSTATE animState;
};