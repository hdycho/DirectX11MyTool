#include "Framework.h"
#include "Weapon.h"

Weapon::Weapon(UINT instanceId,
	wstring matFile,
	wstring meshFile,
	Player*player,
	Effect*lineEffect)
	:GameModel(instanceId, matFile, meshFile, lineEffect)
{
	isLoad = false;
	typeName = L"Weapon";
	this->player = player;
	boneIndex = -2;

	this->matFile = matFile;
	this->meshFile = meshFile;
}

Weapon::Weapon(Weapon & copy, Player*player)
	:GameModel(copy)
{
	typeName = L"Weapon";
	Name() = copy.Name();
	wstring na = copy.Name();
	this->player = player;
	loadBoneIndex = copy.loadBoneIndex;

	this->matFile = copy.matFile;
	this->meshFile = copy.meshFile;
	isLoad = false;

	loadScale = copy.loadScale;
	loadRotate = copy.loadRotate;
	loadPos = copy.loadPos;
}

Weapon::~Weapon()
{
	
}

void Weapon::Ready()
{
	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);

	if (loadBoneIndex!=-2)
	{
		TargetWorld(loadBoneIndex);
		Scale(loadScale);
		Rotation(loadRotate);
		Position(loadPos);
	}

	if (InstanceManager::Get()->FindModel(model->GetModelName()) != NULL)
	{
		wstring modelName = model->GetModelName();
		SAFE_DELETE(model);
		model = InstanceManager::Get()->FindModel(modelName)->GetModel();
	}
	
	if(!isLoad)
		InstanceManager::Get()->AddModel(this, Effects + L"019_ModelInstance.hlsl", InstanceManager::InstanceType::MODEL);

	UpdateWorld();
}

void Weapon::Update()
{
	UpdateWorld();
	__super::Update();
}

void Weapon::Render()
{
	__super::Render();
}

void Weapon::TargetWorld(int boneIdx)
{
	boneIndex = boneIdx;
}

void Weapon::UpdateWorld()
{
	GameRender::UpdateWorld();

	D3DXMATRIX mat, preMat;

	if (boneIndex != -2)
		mat = World()*player->GetPlayerBoneWorld(boneIndex);
	else
		mat = World();


	if (mat != preMat)
	{
	
		if (boneIndex != -2)
		{
			D3DXMATRIX X = player->GetPlayerBoneWorld(boneIndex);
			InstanceManager::Get()->UpdateWorld(this, instanceID, World()*X, InstanceManager::InstanceType::MODEL);
		}
		else
			InstanceManager::Get()->UpdateWorld(this, instanceID,World(), InstanceManager::InstanceType::MODEL);
		

		for (size_t i = 0; i < colliders.size(); i++)
		{
			if (boneIndex != -2)
				colliders[i]->Transform(World()*player->GetPlayerBoneWorld(boneIndex));
			else
				colliders[i]->Transform(World());
		}

		if (boneIndex != -2)
			preMat = World()*player->GetPlayerBoneWorld(boneIndex);
		else
			preMat = World();
	}
}
