#include "stdafx.h"
#include "BoneSphere.h"
#include "../Renders/DepthStencilState.h"

BoneSphere::BoneSphere(ExecuteValues * values)
	:GameModel
	(
		Materials + L"Meshes/", L"Sphere.material", 
		Models + L"Meshes/", L"Sphere.mesh",values
	)
{
	ds[0] = new DepthStencilState();
	ds[1] = new DepthStencilState();

	ds[1]->DepthEnable(false);

	boneScale = { 1,1,1 };
	boneRotate = { 0,0,0 };
	bonePosition = { 0,0,0 };
	
	D3DXMatrixIdentity(&outChangeMat);
}

BoneSphere::~BoneSphere()
{
	SAFE_DELETE(shader);
}

void BoneSphere::Update()
{

	//���� : �ٷ����� �θ� Ŭ����
	__super::Update();
}

void BoneSphere::Render()
{
	ds[1]->OMSetDepthStencilState();
	__super::Render();
	ds[0]->OMSetDepthStencilState();
}

void BoneSphere::UpdateOutMatrix()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, boneScale.x, boneScale.y, boneScale.z);
	//   ������ �ذ� ��� 
	//   1. YawPitchRoll �Թ��� ó��
	//   2. ����� ���(Quaterion) :  
	//   TODO : Quaterion ã�ƺ���

	D3DXMatrixRotationYawPitchRoll(&R, boneRotate.y, boneRotate.x, boneRotate.z);
	D3DXMatrixTranslation(&T, bonePosition.x, bonePosition.y, bonePosition.z);

	outChangeMat = S * R * T;
}


