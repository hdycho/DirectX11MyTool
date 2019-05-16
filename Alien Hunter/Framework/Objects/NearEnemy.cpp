#include "Framework.h"
#include "NearEnemy.h"

#define HURT 1
#define ATTACKAREA 30
#define ATTACKDIST 5

NearEnemy::NearEnemy(Effect * lineEffect)
	:Enemy(lineEffect)
{
	typeName = L"NearEnemy";
	Name() = L"NearEnemy";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 5;
	hp = initHp=4;
	eState = EState::IDLE;

	timeMap[L"IdleTime"] = 0;
	timeMap[L"PatrolTime"] = 0;

	resTimeMap[L"IdleTime"] = Math::Random(3.0f, 10.0f);
	resTimeMap[L"PatrolTime"] = Math::Random(3, 7);

	patrolDirs.push_back(D3DXVECTOR3(1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(1, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(1, 0, -1));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, -1));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, -1));
}

NearEnemy::NearEnemy(UINT instanceId, wstring materialFile, wstring meshFile, Effect * lineEffect)
	:Enemy(instanceId, materialFile, meshFile, lineEffect)
{
	typeName = L"NearEnemy";
	Name() = L"NearEnemy";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	hp = initHp=4;
	eState = EState::IDLE;

	timeMap[L"IdleTime"] = 0;
	timeMap[L"PatrolTime"] = 0;

	resTimeMap[L"IdleTime"] = Math::Random(3.0f, 10.0f);
	resTimeMap[L"PatrolTime"] = Math::Random(3, 7);

	patrolDirs.push_back(D3DXVECTOR3(1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(1, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(1, 0, -1));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, -1));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, -1));
}

NearEnemy::NearEnemy(NearEnemy & copy)
	:Enemy(copy)
{
	typeName = L"NearEnemy";
	Name() = L"NearEnemy";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 5;
	hp = initHp=4;
	eState = EState::IDLE;

	timeMap[L"IdleTime"] = 0;
	timeMap[L"PatrolTime"] = 0;

	resTimeMap[L"IdleTime"] = Math::Random(3.0f, 10.0f);
	resTimeMap[L"PatrolTime"] = Math::Random(3, 7);

	patrolDirs.push_back(D3DXVECTOR3(1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(-1, 0, 0));
	patrolDirs.push_back(D3DXVECTOR3(0.3f, 0, 0.3f));
	patrolDirs.push_back(D3DXVECTOR3(0.3f, 0, -0.3f));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, 1));
	patrolDirs.push_back(D3DXVECTOR3(0, 0, -1));
	patrolDirs.push_back(D3DXVECTOR3(-0.3f, 0, 0.3f));
	patrolDirs.push_back(D3DXVECTOR3(-0.3f, 0, -0.3f));
}

NearEnemy::~NearEnemy()
{
	for (size_t i = 0; i < colliders.size(); i++)
	{
		SAFE_DELETE(colliders[i]);
	}
}

bool NearEnemy::EnemyState(wstring teskName)
{
	if (!InstanceManager::Get()->FindAnim(L"NearEnemy.mesh")->GetRenderState(instanceID))return false;
	if (player == NULL)return false;
	switch (eState)
	{
		case NearEnemy::EState::IDLE:
		{
			StartTargetClip(L" Idle.anim");

			if (Math::Distance(player->GetPosition(), GetPosition()) < ATTACKAREA)
			{
				timeMap[L"IdleTime"] = 0;
				eState = EState::MOVE;
				soundManager::Get()->play("29NearEnemyFind");
			}
			else
			{
				timeMap[L"IdleTime"] += Time::Get()->Delta();
				if (timeMap[L"IdleTime"] > resTimeMap[L"IdleTime"])
				{
					eState = EState::PATROL;
					resTimeMap[L"PatrolTime"]= Math::Random(3, 7);
					timeMap[L"IdleTime"] = 0;
					turnValue = Math::Random(-0.3f, 0.3f);
				}
			}

			if (isDamaged)
			{
				hp--;
				if (hp % 2 == 0 && hp != 0)
				{
					soundManager::Get()->play("25NearEnemyHit", 0.5f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp<=0)
			{
				soundManager::Get()->play("26NearEnemyDeath", 0.5f);
				eState = EState::DIE;
			}
		}
		break;
		case NearEnemy::EState::PATROL:
		{
			StartTargetClip(L" Walking.anim");

			Turn(turnValue);

			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += -Time::Get()->Delta()*Direction()*0.5f;
			Position(curPos);

			if (Math::Distance(player->GetPosition(), GetPosition()) < ATTACKAREA)
			{
				eState = EState::MOVE;
				timeMap[L"PatrolTime"] = 0;
				soundManager::Get()->play("29NearEnemyFind");
			}
			else
			{
				timeMap[L"PatrolTime"] += Time::Get()->Delta();

				if (timeMap[L"PatrolTime"] > resTimeMap[L"PatrolTime"])
				{
					eState = EState::IDLE;
					resTimeMap[L"IdleTime"]= resTimeMap[L"IdleTime"] = Math::Random(3.0f, 10.0f);
					timeMap[L"PatrolTime"] = 0;
				}
			}


			if (isDamaged)
			{
				hp--;
				if (hp % 2 == 0 && hp != 0)
				{
					soundManager::Get()->play("25NearEnemyHit", 0.5f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("25NearEnemyHit", 0.5f);
				eState = EState::DIE;
			}
			
		}
		break;
		case NearEnemy::EState::ATTACK:
		{
			StartTargetClip(L" Attack.anim");

			if (!isOnceAtk)
			{
				soundManager::Get()->play("24NearEnemyAtk", 0.7f);
				isOnceAtk = true;
			}

			if (GetPlayClip() == GetClip(L" Attack.anim"))
			{
				if (GetClip(L" Attack.anim")->IsAnimEnd())
				{
					isOnceAtk = false;
					soundManager::Get()->stop("24NearEnemyAtk");
					eState = EState::MOVE;
				}
			}

			if (isDamaged)
			{
				hp--;
				if (hp % 2 == 0 && hp != 0)
				{
					soundManager::Get()->play("25NearEnemyHit", 0.5f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("26NearEnemyDeath", 0.5f);
				eState = EState::DIE;
			}
		}
		break;
		case NearEnemy::EState::MOVE:
		{
			InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 1, 1, 1));
			StartTargetClip(L" Run.anim");

			D3DXVECTOR3 playerToVec;
			D3DXVECTOR3 rotateXZ = { -Direction().x,0,-Direction().z };

			playerToVec = player->GetPosition() - GetPosition();
			D3DXVec3Normalize(&playerToVec, &playerToVec);

			playerToVec.y = 0;

			float nearAngle = Math::VectorToAngle(rotateXZ, playerToVec);

			if (nearAngle < 0)
				Turn(-1);
			else
				Turn(1);

			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += Time::Get()->Delta()*rotateXZ*1.8f;
			Position(curPos);

			if (Math::Distance(player->GetPosition(), GetPosition()) > ATTACKAREA)
			{
				eState = EState::IDLE;
				timeMap[L"PatrolTime"] = 0;
			}

			if (Math::Distance(player->GetPosition(), GetPosition()) < ATTACKDIST)
			{
				eState = EState::ATTACK;
			}

			if (isDamaged)
			{
				hp--;
				if (hp % 2 == 0 && hp != 0)
				{
					soundManager::Get()->play("25NearEnemyHit", 0.5f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("26NearEnemyDeath", 0.5f);
				eState = EState::DIE;
			}
		}
		break;
		case NearEnemy::EState::DAMAGED:
		{
			if(player->IsFireGun())
				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 0, 0, 1));
			StartTargetClip(L" Damaged.anim");

			if (GetPlayClip() == GetClip(L" Damaged.anim"))
			{
				if (GetClip(L" Damaged.anim")->IsAnimEnd())
				{
					eState = EState::MOVE;
				}
			}
		}
		break;
		case NearEnemy::EState::DIE:
		{
			StartTargetClip(L" Die.anim");

			if (GetPlayClip() == GetClip(L" Die.anim"))
			{
				if (GetClip(L" Die.anim")->IsAnimEnd())
				{
					isDie = true;
					Position(-255, -255, -255);
					InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->SetRenderState(instanceID, false);
				}
			}
		}
		break;
	}
	return false;
}

GData * NearEnemy::Save()
{
	EnemyData*data = new EnemyData();

	data->animData.modelData.modelName = model->GetModelName();
	data->animData.modelData.type = typeName;
	data->animData.modelData.meshFile = meshFile;
	data->animData.modelData.materialFile = matFile;
	data->animData.modelData.materials.assign(model->Materials().begin(), model->Materials().end());
	data->animData.modelData.min = model->Min();
	data->animData.modelData.max = model->Max();
	data->animData.modelData.colliders.assign(colliders.begin(), colliders.end());
	data->animData.modelData.loadFileData = LoadDataFile();

	data->animData.modelData.trails.assign(trails.begin(), trails.end());

	map<wstring, ModelClip*>::iterator mIter = AnimClips().begin();

	for (; mIter != AnimClips().end(); mIter++)
	{
		data->animData.clips.push_back(mIter->second);
	}

	Position(&data->animData.modelData.loadPosition);
	Scale(&data->animData.modelData.loadScale);
	Rotation(&data->animData.modelData.loadRotate);

	data->bt = new BehaviorTree(*BT);

	return (GData*)data;
}

void NearEnemy::Load(wstring fileName)
{
	GameAnimator::Load(fileName);

	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	LoadDataFile() = fileName;

	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement();
	wstring bPoint = String::ToWString(node->GetText());
	while (bPoint != L"BehaviorStart")
	{
		node = node->NextSiblingElement();
		bPoint = String::ToWString(node->GetText());
	}

	BT->ReadBehaviorTree(node, BT->RootNode());

	Play();

	SAFE_DELETE(document);
}

void NearEnemy::ColliderUpdate()
{
	for (size_t i = 0; i < colliders.size(); i++)
	{
 		CollisionResult * result = CollisionContext::Get()->HitTest(colliders[i], CollisionContext::Get()->GetLayer(L"Collision Bullet"), CollisionResult::ResultType::ENearestOne);

		if (result != NULL)
		{
			if (eState != EState::DAMAGED)
			{
				isDamaged = true;
				soundManager::Get()->play("23hit", 0.7f);
				BulletManager::Get()->BulletColliderInit(L"Bullet");
			}
		}

		CollisionResult * result2 = CollisionContext::Get()->HitTest(colliders[i], CollisionContext::Get()->GetLayer(L"Collision PlayerSkill"), CollisionResult::ResultType::ENearestOne);

		if (result2 != NULL)
		{
			if (eState != EState::DAMAGED&&player->IsFireGun())
			{
				isDamaged = true;
			}
		}
	}
}

bool & NearEnemy::LoadStart()
{
	return isLoad;
}

void NearEnemy::UpdateWorld()
{
	GameRender::UpdateWorld();

	InstanceManager::Get()->UpdateWorld(this, instanceID, World(), InstanceManager::InstanceType::ANIMATION);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		//첫번째는 에너미자체콜라이더
		if (i == 0)
			colliders[i]->Transform(World());
		else
			colliders[i]->Transform(GetPlayerBoneWorld(10));
	}
}

void NearEnemy::Turn(float angle)
{
	D3DXVECTOR3 rotate;

	Rotation(&rotate);

	rotate.y += Time::Get()->Delta()*angle;

	rotate.x = 0;
	rotate.z = 0;

	Rotation(rotate);
}
