#include "Framework.h"
#include "Enemy.h"
#include "Model/ModelClip.h"
#include "Collider\ColliderRay.h"
#include "Collider\ColliderBox.h"
#include "Collider\ColliderSphere.h"
#include "Environment\Terrain.h"

Enemy::Enemy(Effect * lineEffect)
	:GameAnimator(lineEffect)
{
	BT = new BehaviorTree();
	terrain = NULL;
	player = NULL;

	UseBoneTransforms();
}

Enemy::Enemy(UINT instanceId, wstring materialFile, wstring meshFile, Effect * lineEffect)
	:GameAnimator(instanceId, materialFile, meshFile, lineEffect)
{
	BT = new BehaviorTree();

	terrain = NULL;
	player = NULL;
	UseBoneTransforms();
}

Enemy::Enemy(Enemy & copy)
	:GameAnimator(copy)
{
	BT = new BehaviorTree(*copy.BT);

	lineEffect = copy.lineEffect;
	terrain = NULL;
	player = NULL;
	UseBoneTransforms();
}

Enemy::~Enemy()
{
	SAFE_DELETE(BT);
}

void Enemy::Ready()
{
	__super::Ready();

}

void Enemy::Update()
{
	if (isDie)return;
	BT->Update(bind(&Enemy::EnemyState, this, placeholders::_1));
	__super::Update();

	if (terrain != NULL)
	{
		D3DXVECTOR3 pPos;
		Position(&pPos);
		Position(pPos.x, terrain->GetHeight(pPos.x, pPos.z), pPos.z);
	}
}

void Enemy::Render()
{
	if (isDie)return;
	__super::Render();
}

GData * Enemy::Save()
{
	return nullptr;
}

void Enemy::Load(wstring fileName)
{

}

void Enemy::ColliderUpdate()
{

}

bool & Enemy::LoadStart()
{
	return isLoad;
}
