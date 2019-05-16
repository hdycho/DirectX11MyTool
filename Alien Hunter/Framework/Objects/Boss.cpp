#include "Framework.h"
#include "Boss.h"

#define HURT 1
#define ATTACKAREA 40
#define ATTACKDIST 5

Boss::Boss(Effect * lineEffect)
	:Enemy(lineEffect)
{
	typeName = L"Boss";
	Name() = L"Boss";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 5;
	hp = initHp = 50;
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

	atkIdx = 0;
	InitAtks();
}

Boss::Boss(UINT instanceId, wstring materialFile, wstring meshFile, Effect * lineEffect)
	:Enemy(instanceId, materialFile, meshFile, lineEffect)
{
	typeName = L"Boss";
	Name() = L"Boss";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 5;
	hp = initHp = 50;
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

	atkIdx = 0;
	InitAtks();
}

Boss::Boss(Boss & copy)
	:Enemy(copy)
{
	typeName = L"Boss";
	Name() = L"Boss";
	behaviors.push_back(L"Move");
	behaviors.push_back(L"Patrol");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Attack");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 5;
	hp = initHp = 50;
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

	atkIdx = 0;
	InitAtks();
}

Boss::~Boss()
{
	for (size_t i = 0; i < colliders.size(); i++)
	{
		SAFE_DELETE(colliders[i]);
	}
}

bool Boss::EnemyState(wstring teskName)
{
	if (!InstanceManager::Get()->FindAnim(L"Boss.mesh")->GetRenderState(instanceID))return false;
	if (player == NULL)return false;

	HpUpdate();
	switch (eState)
	{
		case Boss::EState::IDLE:
		{
			StartTargetClip(L" MutantIdle.anim");

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
					resTimeMap[L"PatrolTime"] = Math::Random(3, 7);
					timeMap[L"IdleTime"] = 0;
					turnValue = Math::Random(-0.3f, 0.3f);
				}
			}

			if (isDamaged)
			{
				hp--;
				if (hp % 10 == 0 && hp != 0)
				{
					soundManager::Get()->play("46BossHit", 0.8f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("45BossDead", 0.8f);
				eState = EState::DIE;
			}
		}
		break;
		case Boss::EState::PATROL:
		{
			StartTargetClip(L" MutantWalk.anim");

			Turn(turnValue);

			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += -Time::Get()->Delta()*Direction()*3.5f;
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
					resTimeMap[L"IdleTime"] = resTimeMap[L"IdleTime"] = Math::Random(3.0f, 10.0f);
					timeMap[L"PatrolTime"] = 0;
				}
			}


			if (isDamaged)
			{
				hp--;
				if (hp % 10 == 0 && hp != 0)
				{
					soundManager::Get()->play("46BossHit", 0.8f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("46BossHit", 0.8f);
				eState = EState::DIE;
			}

		}
		break;
		case Boss::EState::ATTACK:
		{
			switch (bState)
			{
				case Boss::LEVEL1:
				{
					if (atk1[atkIdx] == 1)
					{
						StartTargetClip(L" MutantAttack1.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("41BossAttack1", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack1.anim"))
						{
							if (GetClip(L" MutantAttack1.anim")->IsAnimEnd())
							{
								isOnceAtk = false;
								soundManager::Get()->stop("41BossAttack1");
								
								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
								eState = EState::MOVE;
							}
						}
					}
					else if (atk1[atkIdx] == 2)
					{
						StartTargetClip(L" MutantAttack2.anim");
						
						if (!isOnceAtk)
						{
							soundManager::Get()->play("42BossAttack2", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack2.anim"))
						{
							D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(5)._41,GetPlayerBoneWorld(5)._42-0.5f,GetPlayerBoneWorld(5)._43 };

							if (CurrentKeyFrame() > 47)
							{
								isAttack2 = true;
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f+Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f-Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f+Right()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f - Right()*0.5f, -Direction());
							}

							if (GetClip(L" MutantAttack2.anim")->IsAnimEnd())
							{
								isAttack2 = false;
								ParticleManager::Get()->UnShot(L"PoisonCloud");
								isOnceAtk = false;
								soundManager::Get()->stop("42BossAttack2");

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
								eState = EState::MOVE;
							}
						}
					}
					else
					{
						StartTargetClip(L" MutantAttack3.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("43BossAttack3", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack3.anim"))
						{
							if (CurrentKeyFrame() == 47)
							{
								ParticleManager::Get()->Shot(L"Smoke", D3DXVECTOR3(GetPlayerBoneWorld(1)._41, 1, GetPlayerBoneWorld(1)._43));
								if (!isOnceBomb)
								{
									soundManager::Get()->play("44BossBomb",0.7f);
									isOnceBomb = true;
								}
								isAttack3 = true;
							}
							if (GetClip(L" MutantAttack3.anim")->IsAnimEnd())
							{
								isOnceBomb = false;
								isAttack3 = false;
								isOnceAtk = false;
								soundManager::Get()->stop("43BossAttack3");

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
								eState = EState::MOVE;
							}
						}
					}
				}
				break;
				case Boss::LEVEL2:
				{
					if (atk2[atkIdx] == 1)
					{
						StartTargetClip(L" MutantAttack1.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("41BossAttack1", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack1.anim"))
						{
							if (GetClip(L" MutantAttack1.anim")->IsAnimEnd())
							{
								isOnceAtk = false;
								soundManager::Get()->stop("41BossAttack1");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
					else if (atk2[atkIdx] == 2)
					{
						StartTargetClip(L" MutantAttack2.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("42BossAttack2", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack2.anim"))
						{
							D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(5)._41,GetPlayerBoneWorld(5)._42 - 0.5f,GetPlayerBoneWorld(5)._43 };

							if (CurrentKeyFrame() > 47)
							{
								isAttack2 = true;
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f + Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f - Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f + Right()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f - Right()*0.5f, -Direction());
							}

							if (GetClip(L" MutantAttack2.anim")->IsAnimEnd())
							{
								isAttack2 = false;
								ParticleManager::Get()->UnShot(L"PoisonCloud");

								isOnceAtk = false;
								soundManager::Get()->stop("42BossAttack2");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
					else
					{
						StartTargetClip(L" MutantAttack3.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("43BossAttack3", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack3.anim"))
						{
							if (CurrentKeyFrame() == 47)
							{
								ParticleManager::Get()->Shot(L"Smoke", D3DXVECTOR3(GetPlayerBoneWorld(1)._41, 1, GetPlayerBoneWorld(1)._43));
								
								if (!isOnceBomb)
								{
									soundManager::Get()->play("44BossBomb",0.7);
									isOnceBomb = true;
								}
								isAttack3 = true;
							}
							if (GetClip(L" MutantAttack3.anim")->IsAnimEnd())
							{
								isOnceBomb = false;
								isAttack3 = false;
								isOnceAtk = false;
								soundManager::Get()->stop("43BossAttack3");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
				}
				break;
				case Boss::LEVEL3:
				{
					if (atk3[atkIdx] == 1)
					{
						StartTargetClip(L" MutantAttack1.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("41BossAttack1", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack1.anim"))
						{
							if (GetClip(L" MutantAttack1.anim")->IsAnimEnd())
							{
								isOnceAtk = false;
								soundManager::Get()->stop("41BossAttack1");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
					else if (atk3[atkIdx] == 2)
					{
						StartTargetClip(L" MutantAttack2.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("42BossAttack2", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack2.anim"))
						{
							D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(5)._41,GetPlayerBoneWorld(5)._42 - 0.5f,GetPlayerBoneWorld(5)._43 };

							if (CurrentKeyFrame() > 47)
							{
								isAttack2 = true;
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f + Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f - Up()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f + Right()*0.5f, -Direction());
								ParticleManager::Get()->Shot(L"PoisonCloud", shotPos + (-Direction())*0.2f - Right()*0.5f, -Direction());
							}
							if (GetClip(L" MutantAttack2.anim")->IsAnimEnd())
							{
								isAttack2 = false;
								ParticleManager::Get()->UnShot(L"PoisonCloud");

								isOnceAtk = false;
								soundManager::Get()->stop("42BossAttack2");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
					else
					{
						StartTargetClip(L" MutantAttack3.anim");

						if (!isOnceAtk)
						{
							soundManager::Get()->play("43BossAttack3", 0.8f);
							isOnceAtk = true;
						}

						if (GetPlayClip() == GetClip(L" MutantAttack3.anim"))
						{
							if (CurrentKeyFrame() == 47)
							{
								ParticleManager::Get()->Shot(L"Smoke", D3DXVECTOR3(GetPlayerBoneWorld(1)._41, 1, GetPlayerBoneWorld(1)._43));
								if (!isOnceBomb)
								{
									soundManager::Get()->play("44BossBomb",0.7);
									isOnceBomb = true;
								}
								isAttack3 = true;
							}

							if (GetClip(L" MutantAttack3.anim")->IsAnimEnd())
							{
								isOnceBomb = false;
								isAttack3 = false;
								isOnceAtk = false;
								soundManager::Get()->stop("43BossAttack3");
								eState = EState::MOVE;

								atkIdx++;
								if (atkIdx > 4)
									atkIdx = 0;
							}
						}
					}
				}
				break;
			}
			

			if (isDamaged&&GetPlayClip() != GetClip(L" MutantAttack3.anim"))
			{
				hp--;
				if (hp % 10 == 0 && hp != 0)
				{
					soundManager::Get()->play("46BossHit", 0.8f);
					ParticleManager::Get()->UnShot(L"PoisonCloud");
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				ParticleManager::Get()->UnShot(L"PoisonCloud");
				soundManager::Get()->play("45BossDead", 0.8f);
				eState = EState::DIE;
			}
		}
		break;
		case Boss::EState::MOVE:
		{
			InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 1, 1, 1));
			StartTargetClip(L" MutantRun.anim");

			D3DXVECTOR3 playerToVec;
			D3DXVECTOR3 rotateXZ = { -Direction().x,0,-Direction().z };

			playerToVec = player->GetPosition() - GetPosition();
			D3DXVec3Normalize(&playerToVec, &playerToVec);

			playerToVec.y = 0;

			float nearAngle = Math::VectorToAngle(rotateXZ, playerToVec);

			if (nearAngle < 0)
				Turn(-3.0f);
			else
				Turn(3.0f);

			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += Time::Get()->Delta()*rotateXZ*walkSpeed;
			Position(curPos);

			if (Math::Distance(player->GetPosition(), GetPosition()) > ATTACKAREA)
			{
				eState = EState::IDLE;
				timeMap[L"PatrolTime"] = 0;
			}

			AttackUpdate();

			if (isDamaged)
			{
				hp--;
				if (hp % 10 == 0 && hp != 0)
				{
					soundManager::Get()->play("46BossHit", 0.8f);
					eState = EState::DAMAGED;
				}
				isDamaged = false;
			}

			if (hp <= 0)
			{
				soundManager::Get()->play("45BossDead", 0.8f);
				eState = EState::DIE;
			}
		}
		break;
		case Boss::EState::DAMAGED:
		{
			isAttack2 = false;
			isAttack3 = false;
			soundManager::Get()->stop("42BossAttack2");
			ParticleManager::Get()->UnShot(L"PoisonCloud");
			if (player->IsFireGun())
				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 0, 0, 1));
			StartTargetClip(L" MutantHurt.anim");

			if (GetPlayClip() == GetClip(L" MutantHurt.anim"))
			{
				if (GetClip(L" MutantHurt.anim")->IsAnimEnd())
				{
					eState = EState::MOVE;
				}
			}
		}
		break;
		case Boss::EState::DIE:
		{
			StartTargetClip(L" MutantDying.anim");
			soundManager::Get()->stop("42BossAttack2");
			ParticleManager::Get()->UnShot(L"bossPage2");
			if (GetPlayClip() == GetClip(L" MutantDying.anim"))
			{
				if (GetClip(L" MutantDying.anim")->IsAnimEnd())
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

GData * Boss::Save()
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

void Boss::Load(wstring fileName)
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

void Boss::ColliderUpdate()
{
	for (size_t i = 0; i < 1; i++)
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

bool & Boss::LoadStart()
{
	return isLoad;
}

void Boss::UpdateWorld()
{
	GameRender::UpdateWorld();

	InstanceManager::Get()->UpdateWorld(this, instanceID, World(), InstanceManager::InstanceType::ANIMATION);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		//첫번째는 에너미자체콜라이더
		if (i == 0)
			colliders[i]->Transform(World());
		else if(i==1) // 두번째콜라이더 손에 박힘
			colliders[i]->Transform(GetPlayerBoneWorld(10));
		else // 셋넷콜라이더는 그냥쓴다
			colliders[i]->Transform(World());
	}
}

void Boss::Turn(float angle)
{
	D3DXVECTOR3 rotate;

	Rotation(&rotate);

	rotate.y += Time::Get()->Delta()*angle;

	rotate.x = 0;
	rotate.z = 0;

	Rotation(rotate);
}

void Boss::InitAtks()
{
	atk1[0] = 1;
	atk1[1] = 1;
	atk1[2] = 2;
	atk1[3] = 1;
	atk1[4] = 3;

	atk2[0] = 3;
	atk2[1] = 1;
	atk2[2] = 2;
	atk2[3] = 1;
	atk2[4] = 3;

	atk3[0] = 2;
	atk3[1] = 1;
	atk3[2] = 3;
	atk3[3] = 2;
	atk3[4] = 1;

	bState = LEVEL1;
}

void Boss::HpUpdate()
{
	if (hp > 38)
	{
		bState = LEVEL1;
		walkSpeed = 8;
	}
	else if (hp<=38&&hp>18)
	{
		static bool isOncePage1 = false;
		
		if (!isOncePage1)
		{
			soundManager::Get()->play("47BossPage1");
			isOncePage1 = true;
		}
		bState = LEVEL2;
		walkSpeed = 10;
		ParticleManager::Get()->Shot(L"bossPage1", GetPosition(), Up());
	}
	else if(hp<=18)
	{
		static bool isOncePage2 = false;

		if (!isOncePage2)
		{
			soundManager::Get()->play("48BossPage1");
			isOncePage2 = true;
		}

		bState = LEVEL3;
		walkSpeed = 12;
		ParticleManager::Get()->UnShot(L"bossPage1");
		ParticleManager::Get()->Shot(L"bossPage2", GetPosition(), Up());
	}
}

void Boss::AttackUpdate()
{
	switch (bState)
	{
		case Boss::LEVEL1:
		{
			if (atk1[atkIdx] == 1)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 9)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk1[atkIdx] == 2)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 18)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk1[atkIdx] == 3)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 12)
				{
					eState = EState::ATTACK;

				}
			}
		}
		break;
		case Boss::LEVEL2:
		{
			if (atk2[atkIdx] == 1)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 9)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk2[atkIdx] == 2)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 18)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk2[atkIdx] == 3)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 12)
				{
					eState = EState::ATTACK;
	
				}
			}
		}
		break;
		case Boss::LEVEL3:
		{
			if (atk3[atkIdx] == 1)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 9)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk3[atkIdx] == 2)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 18)
				{
					eState = EState::ATTACK;

				}
			}
			else if (atk3[atkIdx] == 3)
			{
				if (Math::Distance(player->GetPosition(), GetPosition()) < 12)
				{
					eState = EState::ATTACK;

				}
			}
		}
		break;
	}
}
