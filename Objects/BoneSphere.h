#pragma once
#include "GameModel.h"

class BoneSphere :public GameModel
{
public:
	BoneSphere(ExecuteValues * values);
	~BoneSphere();

	void Update() override;
	void Render() override;

	D3DXVECTOR3&BoneScale() { return boneScale; }
	D3DXVECTOR3&BoneRotate() { return boneRotate; }
	D3DXVECTOR3&BonePosition() { return bonePosition; }

	void UpdateOutMatrix();
	D3DXMATRIX&GetOutMatrix() { return outChangeMat; }
private:
	class DepthStencilState*ds[2];

	//�ӱ��̼��ÿ��尪
	D3DXVECTOR3 boneScale;
	D3DXVECTOR3 boneRotate;
	D3DXVECTOR3 bonePosition;


	D3DXMATRIX outChangeMat; // �ܺο������� ���尪
};