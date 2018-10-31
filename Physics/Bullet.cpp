#include "Bullet.h"
#include "./Objects/GameModel.h"
#include "./Physics/ColliderBox.h"

Bullet*Bullet::instance = NULL;

Bullet * Bullet::GetInstance()
{
	if (instance == NULL)
		instance = new Bullet;

	return instance;
}

void Bullet::BulletSetting(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, int bNum, wstring bulletName)
{
	vector<bulletModel*> gVec;

	for (int i = 0; i < bNum; i++)
	{
		bulletModel*bm = new bulletModel;
		bm->model = new GameModel(matFolder, matFile, meshFolder, meshFile,NULL);
		bm->collider = new ColliderBox(bm->model->World(), BOX);
		Shader*shader= new Shader(Shaders + L"009_Model.hlsl");
		//ÃÑ¾Ë½¦ÀÌ´õ ¼³Á¤
		for (Material*material: bm->model->GetModel()->Materials())
		{
			material->SetShader(shader);
		}
		bm->isShot = false;
		gVec.push_back(bm);
	}

	bMap[bulletName] = gVec;
	bulletIdxMap[bulletName] = 0;
}

void Bullet::Update()
{
	bulletMap::iterator bIter = bMap.begin();

	for (; bIter != bMap.end(); bIter++)
	{
		for (int i = 0; i < bIter->second.size(); i++)
		{
			if (!bIter->second[i]->isShot)continue;
			bIter->second[i]->model->Update();
			if (bIter->second[i]->gravity != 0.0f)
			{
				bIter->second[i]->addGravity += bIter->second[i]->gravity*0.001f;

				D3DXVECTOR3 curPos = bIter->second[i]->model->Position();
				curPos.y -= bIter->second[i]->addGravity;
				bIter->second[i]->model->Position(curPos);
			}
			bIter->second[i]->collider->Update(bIter->second[i]->model->World());
		}
	}
}

void Bullet::Render()
{
	bulletMap::iterator bIter = bMap.begin();

	for (; bIter != bMap.end(); bIter++)
	{
		for (int i = 0; i < bIter->second.size(); i++)
		{
			if (!bIter->second[i]->isShot)continue;
			bIter->second[i]->model->Render();
			bIter->second[i]->collider->Render();
		}
	}
}

void Bullet::Release()
{
}

void Bullet::Shot(wstring bulletName, D3DXVECTOR3 shotPos, D3DXVECTOR3 shotVector, float speed, float gravity)
{
	bulletMap::iterator bIter = bMap.find(bulletName);

	shotVector.x *= speed;
	shotVector.y *= speed;
	shotVector.z *= speed;
	bIter->second[bulletIdxMap[bulletName]]->gravity = gravity;
	bIter->second[bulletIdxMap[bulletName]]->addGravity = 0;
	bIter->second[bulletIdxMap[bulletName]]->isShot = true;
	bIter->second[bulletIdxMap[bulletName]]->model->Position(shotPos);
	bIter->second[bulletIdxMap[bulletName]]->model->Velocity(shotVector);

	bulletIdxMap[bulletName]++;

	if (bulletIdxMap[bulletName] > bIter->second.size() - 1)
		bulletIdxMap[bulletName] = 0;

	return;
}

void Bullet::RenewColliderBox(wstring bulletName, int i)
{
	GetBullet(bulletName)[i]->collider->RenewBoundingBox(GetBullet(bulletName)[i]->model->World());
}
