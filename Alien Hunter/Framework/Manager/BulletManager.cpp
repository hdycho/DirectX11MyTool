#include "Framework.h"
#include "BulletManager.h"

BulletManager*BulletManager::instance = NULL;

BulletManager * BulletManager::Get()
{
	if (instance == NULL)
		instance = new BulletManager();
	return instance;
}

void BulletManager::Delete()
{
	SAFE_DELETE(instance);
}

BulletManager::BulletManager()
{

}

BulletManager::~BulletManager()
{
}

void BulletManager::BulletInit(wstring bulletName, wstring dataFile, UINT bulletNum, float speed, float range, bool isGravity)
{
	for (UINT i = 0; i < bulletNum; i++)
	{
		GameModel*bullet = new GameModel(i, L"", L"");

		
		bullet->Load(Datas + dataFile);
		bullet->Ready();

		BulletDesc desc;

		desc.bullet = bullet;
		desc.isGravity = isGravity;
		desc.range = range;
		desc.speed = speed;
		desc.isShot = false;

		bulletMap[bulletName].push_back(desc);
	}

	for (UINT i = 0; i < bulletNum; i++)
	{
		ColliderManager::Get()->InputObject(bulletMap[bulletName][i].bullet);
		ColliderManager::Get()->AddLayerToCollider(L"Collision Bullet", bulletMap[bulletName][i].bullet->GetColliders()[0]);
	}
}

void BulletManager::BulletUpdate()
{
	bulletIter bIter = bulletMap.begin();

	for (; bIter != bulletMap.end(); bIter++)
	{
		for (size_t i = 0; i < bIter->second.size(); i++)
		{
			if (!bIter->second[i].isShot)continue;

			D3DXVECTOR3 curPos;
			bIter->second[i].bullet->Position(&curPos);
			
			curPos += -Time::Get()->Delta()*bIter->second[i].dir*bIter->second[i].speed;

			bIter->second[i].bullet->Position(curPos);

			bIter->second[i].curPos = curPos;
			bIter->second[i].bullet->Update();

			ParticleManager::Get()->Shot(L"bulletEffect",curPos, bIter->second[i].bullet->Direction());

			CollisionResult * result = CollisionContext::Get()->HitTest(bIter->second[i].bullet->GetColliders()[0], CollisionContext::Get()->GetLayer(L"Collision Enemy"), CollisionResult::ResultType::ENearestOne);

			if (result != NULL)
			{
				if (!bIter->second[i].isThrough)
				{
					InstanceManager::Get()->FindModel(bIter->second[i].bullet->GetModel()->GetModelName())->SetRenderState(i, false);
					bIter->second[i].isShot = false;
					ParticleManager::Get()->UnShot(L"bulletEffect");
					ParticleManager::Get()->Shot(L"Blood", bIter->second[i].curPos, bIter->second[i].dir);
					bIter->second[i].bullet->GetTrails()[0]->ResetTrail();
				}
			}
			
			if (Math::Distance(bIter->second[i].curPos, bIter->second[i].startPos)>25)
			{
				ParticleManager::Get()->UnShot(L"bulletEffect");
				bIter->second[i].bullet->GetTrails()[0]->ResetTrail();
				InstanceManager::Get()->FindModel(bIter->second[i].bullet->GetModel()->GetModelName())->SetRenderState(i, false);
				bIter->second[i].isShot = false;
				D3DXMATRIX mat;
				D3DXMatrixIdentity(&mat);

				mat._41 = mat._42 = mat._43 = 100;
				bIter->second[i].bullet->GetColliders()[0]->Transform(mat);
			}
		}
	}
}

void BulletManager::BulletRender()
{
	bulletIter bIter = bulletMap.begin();

	for (; bIter != bulletMap.end(); bIter++)
	{
		for (size_t i = 0; i < bIter->second.size(); i++)
		{
			if (!bIter->second[i].isShot)continue;

			bIter->second[i].bullet->Render();
		}
	}
}

void BulletManager::ReloadBullet(wstring bulletName, D3DXVECTOR3 startPos)
{
	bulletIter bIter = bulletMap.find(bulletName);

	for (UINT i = 0; i < bIter->second.size(); i++)
	{
		bIter->second[i].bullet->GetTrails()[0]->ResetTrail();
		bIter->second[i].startPos = startPos;
		bIter->second[i].bullet->Position(startPos);
		InstanceManager::Get()->FindModel(bIter->second[i].bullet->GetModel()->GetModelName())->SetRenderState(i, false);
		bIter->second[i].isShot = false;
	}
}

void BulletManager::Shot(wstring bulletName, D3DXVECTOR3 dir, D3DXVECTOR3 startPos, D3DXVECTOR3 rotate, float speed, float size, bool isThrough)
{
	bulletIter bIter = bulletMap.find(bulletName);

	for (UINT i = 0; i < bIter->second.size(); i++)
	{
		if (bIter->second[i].isShot)continue;

		InstanceManager::Get()->FindModel(bIter->second[i].bullet->GetModel()->GetModelName())->SetRenderState(i, true);
		bIter->second[i].dir = dir;
		bIter->second[i].dir.y -= 0.05f;
		bIter->second[i].isShot = true;
		bIter->second[i].speed = speed;
		bIter->second[i].startPos = startPos;
		bIter->second[i].bullet->Position(startPos);
		bIter->second[i].bullet->Rotation(rotate);
		bIter->second[i].bullet->Scale(size, size, size);
		bIter->second[i].isThrough = isThrough;

		break;
	}
}

void BulletManager::BulletColliderInit(wstring bulletName)
{
	bulletIter bIter = bulletMap.find(bulletName);

	for (UINT i = 0; i < bIter->second.size(); i++)
	{
		if (bIter->second[i].isShot)continue;

		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		mat._41 = mat._42 = mat._43 = 100;
		bIter->second[i].bullet->GetColliders()[0]->Transform(mat);
	}
}

void BulletManager::RemoveAll()
{
	bulletIter biter = bulletMap.begin();

	for (; biter != bulletMap.end(); biter++)
	{
		for (size_t i = 0; i < biter->second.size(); i++)
		{
			SAFE_DELETE(biter->second[i].bullet);
		}
		biter->second.clear();
	}

	bulletMap.clear();
}
