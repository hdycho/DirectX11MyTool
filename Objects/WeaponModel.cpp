#include "stdafx.h"
#include "WeaponModel.h"

WeaponModel::WeaponModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, ExecuteValues * values)
	:GameModel(matFolder, matFile, meshFolder, meshFile, values)
{
	parentModel = NULL;
	parentName = L"None";
	myPtr = this;
	className = L"Weapon";
}

WeaponModel::WeaponModel(WeaponModel & model)
	:GameModel(model)
{
	parentModel = NULL;
	parentName = L"None";
	myPtr = this;
	className = L"Weapon";
}

WeaponModel::~WeaponModel()
{
}

void WeaponModel::Update()
{
	//슈퍼 : 바로위에 부모 클래스
	if (parentModel != NULL)
	{
		D3DXMATRIX result = parentModel->GetBoneMatrix(parentName);
		D3DXMATRIX S, R, T;

		D3DXMATRIX scale, trans, rotate;
		D3DXMatrixIdentity(&scale);
		scale._11 = Scale().x;
		scale._22 = Scale().y;
		scale._33 = Scale().z;

		D3DXMatrixIdentity(&trans);
		trans._41 = Position().x;
		trans._42 = Position().y;
		trans._43 = Position().z;

		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationYawPitchRoll(&rotate, Rotation().y, Rotation().x, Rotation().z);

		result = scale*rotate*trans*result;
		World(result);
	}
	__super::Update();
}

void WeaponModel::Render()
{
	__super::Render();
}
