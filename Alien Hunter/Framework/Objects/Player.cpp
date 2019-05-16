#include "Framework.h"
#include "Player.h"
#include "Model/ModelClip.h"
#include "Collider\ColliderRay.h"
#include "Collider\ColliderBox.h"
#include "Collider\ColliderSphere.h"
#include "Environment\Terrain.h"
#include "Viewer\TPSCamera.h"
#include "Environment\ParticleSystem.h"

Player::Player(Effect*lineEffect)
	:GameAnimator(lineEffect)
{
	cam = NULL;
	weaponState = RAYCASTGUN;

	crossVec = { 0,0,0 };
	rebound = 0;

	typeName = L"Player";
	Name() = L"Player";
	BT = new BehaviorTree();

	behaviors.push_back(L"Move");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Aim");
	behaviors.push_back(L"Run");
	behaviors.push_back(L"Diving");
	behaviors.push_back(L"WeaponChange");
	behaviors.push_back(L"Reload");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");


	BT->InputBehaviorsName(behaviors);

	walkSpeed = 13;
	runSpeed = 23;
	camRotate = { Math::ToRadian(10),0,0 };

	terrain = NULL;
	UseBoneTransforms();
}

Player::Player(UINT instanceId, wstring materialFile, wstring meshFile, Effect * lineEffect)
	:GameAnimator(instanceId, materialFile, meshFile, lineEffect)
{
	cam = NULL;
	weaponState = RAYCASTGUN;

	crossVec = { 0,0,0 };
	rebound = 0;

	typeName = L"Player";
	Name() = L"Player";
	BT = new BehaviorTree();

	behaviors.push_back(L"Move");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Aim");
	behaviors.push_back(L"Run");
	behaviors.push_back(L"Diving");
	behaviors.push_back(L"WeaponChange");
	behaviors.push_back(L"Reload");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	walkSpeed = 13;
	runSpeed = 23;
	camRotate = { Math::ToRadian(10),0,0 };

	terrain = NULL;
	UseBoneTransforms();

}

Player::Player(Player & copy)
	:GameAnimator(copy)
{
	cam = NULL;
	weaponState = RAYCASTGUN;

	crossVec = { 0,0,0 };
	rebound = 0;

	typeName = L"Player";
	Name() = L"Player";
	BT = new BehaviorTree(*copy.BT);

	behaviors.push_back(L"Move");
	behaviors.push_back(L"Idle");
	behaviors.push_back(L"Aim");
	behaviors.push_back(L"Run");
	behaviors.push_back(L"Diving");
	behaviors.push_back(L"WeaponChange");
	behaviors.push_back(L"Reload");
	behaviors.push_back(L"Damaged");
	behaviors.push_back(L"Die");

	BT->InputBehaviorsName(behaviors);

	for (size_t i = 0; i < copy.weapons.size(); i++)
	{
		Weapon*weapon = new Weapon(*copy.weapons[i], this);
		weapons.push_back(weapon);
	}

	lineEffect = copy.lineEffect;

	walkSpeed = 13;
	runSpeed = 23;
	camRotate = { Math::ToRadian(10),0,0 };

	terrain = NULL;
	UseBoneTransforms();

}

Player::~Player()
{
	SAFE_DELETE(BT);

	for (UINT i = 0; i < weapons.size(); i++)
		SAFE_DELETE(weapons[i]);
}

void Player::Ready()
{
	__super::Ready();

	for (size_t i = 0; i < weapons.size(); i++)
		weapons[i]->Ready();

	D3DXCOLOR color = model->Materials()[0]->GetSpecular();

	color.a = 50;
	model->Materials()[0]->SetSpecular(color);

	if (InstanceManager::Get()->FindModel(L"mGun.mesh") != NULL)
	{
		InstanceManager::Get()->FindModel(L"mGun.mesh")->SetRenderState(instanceID, false);
		InstanceManager::Get()->FindModel(L"sGun.mesh")->SetRenderState(instanceID, false);
	}
	/*InstanceManager::Get()->FindModel(L"mGun.mesh")->SetRenderState(instanceID, false);
	InstanceManager::Get()->FindModel(L"sGun.mesh")->SetRenderState(instanceID, false);*/

}

void Player::Update()
{
	//ParticleManager::Get()->Shot(L"DashEffect", GetPosition());
	if(isUpdate)
		BT->Update(bind(&Player::PlayerState, this, placeholders::_1));
	__super::Update();

	camPos = { GetPlayerBoneWorld(1)._41,GetPlayerBoneWorld(1)._42-0.3f,GetPlayerBoneWorld(1)._43 };


	for (size_t i = 0; i <2; i++)
		weapons[i]->Update();

	weapons[weaponState]->Update();

	if (terrain != NULL)
	{
		D3DXVECTOR3 pPos;
		Position(&pPos);
		Position(pPos.x, terrain->GetHeight(pPos.x, pPos.z), pPos.z);
	}

	UiUpdate();
}

void Player::Render()
{
	__super::Render();


	weapons[weaponState]->Render();
}

void Player::UiUpdate()
{
	if (weaponState == WeaponState::RAYCASTGUN)
	{
		UIManager::Get()->SetUiRender(L"gun1", true);
		UIManager::Get()->SetUiRender(L"gun2", false);
		UIManager::Get()->SetUiRender(L"gun3", false);

		UIManager::Get()->SetUiRender(L"bullet3", false);
		for (int i = 1; i <= 10; i++)
			UIManager::Get()->SetUiRender(L"bullet2_" + to_wstring(i), false);

		for (int i = 1; i <= bullet1; i++)
			UIManager::Get()->SetUiRender(L"bullet1_" + to_wstring(i), true);

		if (bullet1 < 2)
		{
			for (int i = bullet1 + 1; i <= 2; i++)
				UIManager::Get()->SetUiRender(L"bullet1_" + to_wstring(i), false);
		}
	}
	else if (weaponState == WeaponState::MUCHINGUN)
	{
		UIManager::Get()->SetUiRender(L"gun1", false);
		UIManager::Get()->SetUiRender(L"gun2", true);
		UIManager::Get()->SetUiRender(L"gun3", false);

		UIManager::Get()->SetUiRender(L"bullet3", false);
		for (int i = 1; i <= 2; i++)
			UIManager::Get()->SetUiRender(L"bullet1_" + to_wstring(i), false);

		for (int i = 1; i <= bullet2; i++)
			UIManager::Get()->SetUiRender(L"bullet2_" + to_wstring(i), true);

		if (bullet2 < 10)
		{
			for (int i = bullet2 + 1; i <= 10; i++)
				UIManager::Get()->SetUiRender(L"bullet2_" + to_wstring(i), false);
		}
	}
	else if (weaponState == WeaponState::FIREGUN)
	{
		UIManager::Get()->SetUiRender(L"gun1", false);
		UIManager::Get()->SetUiRender(L"gun2", false);
		UIManager::Get()->SetUiRender(L"gun3", true);


		UIManager::Get()->SetUiRender(L"bullet3", true);

		for (int i = 1; i <= 10; i++)
			UIManager::Get()->SetUiRender(L"bullet2_" + to_wstring(i), false);
		for (int i = 1; i <= 2; i++)
			UIManager::Get()->SetUiRender(L"bullet1_" + to_wstring(i), false);

	}

	for (int i = 1; i <= hp; i++)
		UIManager::Get()->SetUiRender(L"hp"+to_wstring(i),true);
	
	if (hp < 8)
	{
		for (int i = hp + 1; i <= 8; i++)
			UIManager::Get()->SetUiRender(L"hp" + to_wstring(i), false);
	}
}


bool Player::PlayerState(wstring teskName)
{
	if (terrain != NULL)
		Turn();
	if (teskName == L"Aim")
	{
		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (Mouse::Get()->Press(1))
		{
			if (!isZoom)
			{
				soundManager::Get()->play("14Zoomin",0.5f);
				isZoom = true;
			}
			if(cam!=NULL)
				cam->UseToZoom(true, D3DXVECTOR3(1.15f, 0.43f, 6.75f));

			UIManager::Get()->SetUiRender(L"search", true);
			
			if (weaponState == RAYCASTGUN)
			{
				UIManager::Get()->SetUiRender(L"cross2", true);
				if (Mouse::Get()->Down(0)&&bullet1>0&& !isOneShot)
				{
					StartTargetClip(L" Shot.anim");
					isOneShot = true;
					bullet1--;
					
					return true;
				}
				else
				{
					if (isOneShot)
					{
						static bool oneShot1 = false;
						if (CurrentKeyFrame() == 4)
						{
							if (!oneShot1)
							{
								soundManager::Get()->play("09Gun2", 0.7f);
								oneShot1 = true;
							}
							D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.05f,GetPlayerBoneWorld(15)._43 };
							
							BulletManager::Get()->Shot(L"Bullet", Direction(), shotPos, GetRotation(), 45, 6.0f);

							D3DXVECTOR3 shotVec1 = { Direction().x,Direction().y+0.05f,Direction().z };
							BulletManager::Get()->Shot(L"Bullet", shotVec1, shotPos, GetRotation(), 45, 6.0f);

							shotVec1 = { Direction().x,Direction().y - 0.05f,Direction().z };
							BulletManager::Get()->Shot(L"Bullet", shotVec1, shotPos, GetRotation(), 45, 6.0f);

							shotVec1 = { Direction().x - 0.05f,Direction().y ,Direction().z };
							BulletManager::Get()->Shot(L"Bullet", shotVec1, shotPos, GetRotation(), 45, 6.0f);

							shotVec1 = { Direction().x + 0.05f,Direction().y,Direction().z };
							BulletManager::Get()->Shot(L"Bullet", shotVec1, shotPos, GetRotation(), 45, 6.0f);

							shotVec1 = { Direction().x + 0.05f,Direction().y,Direction().z };
							BulletManager::Get()->Shot(L"Bullet", shotVec1, shotPos, GetRotation(), 45, 4.5f);

							shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.3f,GetPlayerBoneWorld(15)._43 };
							ParticleManager::Get()->Shot(L"gunfire", shotPos + (-Direction())*1.1f, -Direction());
						}
						else
							oneShot1 = false;
					}

					if (GetClip(L" Shot.anim")->IsAnimEnd())
					{
						isOneShot = false;
					}
				}
			}
			else
			{
				if(weaponState == MUCHINGUN)UIManager::Get()->SetUiRender(L"cross1", true);
				else if(weaponState == FIREGUN)UIManager::Get()->SetUiRender(L"cross3", true);
				static bool oneShot3 = false;
				if (Mouse::Get()->Press(0))
				{
					if (weaponState == MUCHINGUN)
					{
						if (bullet2 > 0)
						{
							static bool oneShot2 = false;
							if (CurrentKeyFrame() == 2)
							{
								if (!isOneShot)
								{
									if (!oneShot2)
									{
										soundManager::Get()->play("08Gun1", 0.5f);
										oneShot2 = true;
									}
									bullet2--;
									D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.25f,GetPlayerBoneWorld(15)._43 };
									BulletManager::Get()->Shot(L"Bullet", Direction(), shotPos, GetRotation(), 50, 6);

									shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.3f,GetPlayerBoneWorld(15)._43 };
									ParticleManager::Get()->Shot(L"gunfire", shotPos + (-Direction())*1.5f, -Direction());
									isOneShot = true;
								}
								rebound = 0.0013f;
							}
							else
							{
								oneShot2 = false;
								rebound = 0;
								isOneShot = false;
							}
						}
						else
						{
							rebound = 0;
						}
					}
					else if (weaponState == FIREGUN)
					{
						if (!oneShot3)
						{
							soundManager::Get()->play("10Gun3", 0.5f);
							oneShot3 = true;
						}
						D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.3f,GetPlayerBoneWorld(15)._43 };

						D3DXVECTOR3 forward = -Direction();

						isFireGun = true;
						ParticleManager::Get()->Shot(L"Fire",shotPos+(-Direction())*2.7f,-Direction());
					}
					StartTargetClip(L" Shot2.anim");
					return true;
				}
				else
				{
					soundManager::Get()->stop("13EmptyShot");
					oneShot3 = false;
					soundManager::Get()->stop("10Gun3");
					rebound = 0;
					isFireGun = false;
					ParticleManager::Get()->UnShot(L"Fire");
				}
			}

			if (GetPlayClip() != GetClip(L" Shot.anim"))
			{
				StartTargetClip(L" Aim.anim");
				return true;
			}
			else
			{
				if (CurrentKeyFrame() == 5)
					rebound = 0.0105f;
				else
					rebound = 0;

				if (GetClip(L" Shot.anim")->IsAnimEnd())
				{
					isOneShot = false;
					if (Mouse::Get()->Press(1))
					{
						StartTargetClip(L" Aim.anim");
						return true;
					}

					return false;
				}
			}
		}
		else
		{
			if (cam != NULL)
				cam->UseToZoom(false, D3DXVECTOR3(0, 0, 0));

			UIManager::Get()->SetUiRender(L"search", false);

			if(isZoom)
			{
				soundManager::Get()->play("15Zoomout",0.5f);
				isZoom = false;
			}
			UIManager::Get()->SetUiRender(L"cross1", false);
			UIManager::Get()->SetUiRender(L"cross2", false);
			UIManager::Get()->SetUiRender(L"cross3", false);
			isOneShot = false;
			return false;
		}
	}
	else if (teskName == L"Move")
	{
		return MoveTPS();
	}
	else if (teskName == L"Run")
	{
		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (Keyboard::Get()->Press(VK_SHIFT))
		{
			if (Keyboard::Get()->Press('W'))
			{
				if (Keyboard::Get()->Down(VK_SPACE))return false;
				if (GetPlayClip() == GetClip(L" Diving.anim"))return false;

				D3DXVECTOR3 curPos;
				Position(&curPos);

				curPos += -Time::Delta()*Direction() * runSpeed;
				Position(curPos);

				StartTargetClip(L" Run.anim");
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else if (teskName == L"Diving")
	{
		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (Keyboard::Get()->Down(VK_SPACE))
		{
			soundManager::Get()->play("40hop");
			StartTargetClip(L" Diving.anim");
			return true;
		}
		else
		{
			if (GetClip(L" Diving.anim")->IsAnimEnd())
			{
				return false;
			}

			if (GetPlayClip() != GetClip(L" Diving.anim"))
				return false;
		}
	}
	else if (teskName == L"WeaponChange")
	{
		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (Keyboard::Get()->Down('1')|| Keyboard::Get()->Down('2')|| Keyboard::Get()->Down('3'))
		{
			isOneShot = false;
			D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.05f,GetPlayerBoneWorld(15)._43 };
			BulletManager::Get()->ReloadBullet(L"Bullet",shotPos);

			if (Keyboard::Get()->Down('1'))
			{
				//이미 1번총이면 바로탈출
				if (weaponState == RAYCASTGUN)return false;
				soundManager::Get()->play("12ChangeGun");
				weaponState = RAYCASTGUN;
				bullet1 = 2;
				
				InstanceManager::Get()->FindModel(L"Gun.mesh")->SetRenderState(0, true);
				InstanceManager::Get()->FindModel(L"mGun.mesh")->SetRenderState(0, false);
				InstanceManager::Get()->FindModel(L"sGun.mesh")->SetRenderState(0, false);
			}
			else if (Keyboard::Get()->Down('2'))
			{
				//이미 2번총이면 바로탈출
				if (weaponState == MUCHINGUN)return false;
				soundManager::Get()->play("12ChangeGun");
				weaponState = MUCHINGUN;
				bullet2 = 10;

				InstanceManager::Get()->FindModel(L"Gun.mesh")->SetRenderState(0, false);
				InstanceManager::Get()->FindModel(L"mGun.mesh")->SetRenderState(0, true);
				InstanceManager::Get()->FindModel(L"sGun.mesh")->SetRenderState(0, false);
			}
			else if (Keyboard::Get()->Down('3'))
			{
				//이미 2번총이면 바로탈출
				if (weaponState == FIREGUN)return false;
				soundManager::Get()->play("12ChangeGun");
				weaponState = FIREGUN;

				InstanceManager::Get()->FindModel(L"Gun.mesh")->SetRenderState(0, false);
				InstanceManager::Get()->FindModel(L"mGun.mesh")->SetRenderState(0, false);
				InstanceManager::Get()->FindModel(L"sGun.mesh")->SetRenderState(0, true);
			}
			StartTargetClip(L" WeaponChange.anim");
			return true;
		}
		else
		{
			if (GetClip(L" WeaponChange.anim")->IsAnimEnd())
			{
				CurrentKeyFrame() = 22;
				return false;
			}
			if (GetPlayClip() != GetClip(L" WeaponChange.anim"))
				return false;
		}
	}
	else if (teskName == L"Reload")
	{
		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (Keyboard::Get()->Down('R'))
		{
			soundManager::Get()->play("11Reload");
			isOneShot = false;
			D3DXVECTOR3 shotPos = { GetPlayerBoneWorld(15)._41,GetPlayerBoneWorld(15)._42 + 0.05f,GetPlayerBoneWorld(15)._43 };
			BulletManager::Get()->ReloadBullet(L"Bullet", shotPos);
			bullet1 = 2;
			bullet2 = 10;
			StartTargetClip(L" Reload.anim");
			return true;
		}
		else
		{
			if (GetClip(L" Reload.anim")->IsAnimEnd())
			{
				return false;
			}
			if (GetPlayClip() != GetClip(L" Reload.anim"))
				return false;
		}
	}
	else if (teskName == L"Damaged")
	{
		if (hp <= 0 && !isDie)
		{
			if (GetPlayClip() != GetClip(L" Die.anim"))
			{
				soundManager::Get()->play("28PlayerDeath");
				isDie = true;
				StartTargetClip(L" Die.anim");
			}
			return false;
		}

		if (isDamaged&&!isDie)
		{
			static bool onceDamaged = false;
			if (!onceDamaged)
			{
				soundManager::Get()->play("27PlayerHurt");
				onceDamaged = true;
			}
			
			D3DXVECTOR3 curPos = GetPosition();

			curPos += Direction()*Time::Get()->Delta()*hitValue;

			Position(curPos);

			if (hitValue > 0)
				hitValue -= Time::Get()->Delta()*19;
			

			UIManager::Get()->SetUiRender(L"bloodScreen", true);
			if (GetClip(L" Damaged.anim")->IsAnimEnd())
			{
				hitValue = 20;
				UIManager::Get()->SetUiRender(L"bloodScreen", false);
				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 1, 1, 1));
				isDamaged = false;
				onceDamaged = false;
				hp -= 1;
				return true;
			}
		}
		else
		{
			return false;
		}

		return false;
	}
	else if (teskName == L"Die")
	{
		if (hp <= 0)
		{
			return true;
		}
		else
			return false;
	}
	else if (teskName == L"Idle") // 이건맨마지막으로 넣어야함
	{
		if (Keyboard::Get()->Press('W') ||
			Keyboard::Get()->Press('S') ||
			Keyboard::Get()->Press('A') ||
			Keyboard::Get()->Press('D')||
			isDamaged||
			hp<=0)
		{
			return false;
		}
		else
		{
			StartTargetClip(L" Idle.anim");
			return true;
		}
	}
	return true;
}

vector<Weapon*>& Player::GetWeapons()
{
	return weapons;
}

GData * Player::Save()
{
	PlayerData*data = new PlayerData();

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
	data->weapons.assign(weapons.begin(), weapons.end());

	return (GData*)data;
}

void Player::Load(wstring fileName)
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
	while (bPoint != L"Weapon")
	{
		node = node->NextSiblingElement();
		bPoint = String::ToWString(node->GetText());
	}

	node = node->NextSiblingElement(); // WeaponCount
	UINT weaponCount = node->IntText();

	if(weaponCount!=0)
		node = node->NextSiblingElement();
	for (UINT i = 0; i < weaponCount; i++)
	{
		Xml::XMLElement*weaponElement = node->FirstChildElement(); // FileName
		wstring materialFile = String::ToWString(weaponElement->GetText());

		weaponElement = weaponElement->NextSiblingElement();
		wstring meshFile = String::ToWString(weaponElement->GetText());

		weaponElement = weaponElement->NextSiblingElement(); // Name;
		wstring name = String::ToWString(weaponElement->GetText());

		UINT nameIdx = 0;
		UINT instNumber = 0;
		while (true)
		{
			if (name[nameIdx] == '0'|| name[nameIdx] == '1'|| name[nameIdx] == '2'||
				name[nameIdx] == '3'|| name[nameIdx] == '4'|| name[nameIdx] == '5')
			{
				instNumber = name[nameIdx] - 48;
				break;
			}
			nameIdx++;
		}

		weaponElement = weaponElement->NextSiblingElement(); // boneIndex;
		int boneIndex = weaponElement->IntText();

		D3DXVECTOR3 min, max;
		D3DXVECTOR3 pos, scale, rotate;

		weaponElement = weaponElement->NextSiblingElement(); // minx;
		min.x = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // miny;
		min.y = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // minz;
		min.z = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // maxx;
		max.x = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // maxy;
		max.y = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // maxz;
		max.z = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // posx;
		pos.x = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // posy;
		pos.y = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // posz;
		pos.z = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // scalex;
		scale.x = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // scaley;
		scale.y = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // scalez;
		scale.z = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // rotatex;
		rotate.x = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // rotatey;
		rotate.y = weaponElement->FloatText();

		weaponElement = weaponElement->NextSiblingElement(); // rotatez;
		rotate.z = weaponElement->FloatText();

		Weapon*weapon = new Weapon
		(
			instNumber,
			materialFile,
			meshFile,
			this,
			lineEffect
		);

		weapon->Name() = name;
		weapon->loadBoneIndex = boneIndex;
		weapon->loadScale = scale;
		weapon->loadRotate = rotate;
		weapon->loadPos = pos;
		weapon->LoadStart() = true;

		weaponElement = weaponElement->NextSiblingElement(); // WColliderSize;
		int weaponColSize = weaponElement->IntText();

		if(weaponColSize!=0)
			weaponElement = weaponElement->NextSiblingElement(); // Colliders
		for (int j = 0; j < weaponColSize; j++)
		{
			Xml::XMLElement*col = weaponElement->FirstChildElement(); // Type
			wstring typeName = String::ToWString(col->GetText());

			col = col->NextSiblingElement();
			wstring name = String::ToWString(col->GetText());

			if (typeName == L"Box")
			{
				ColliderBox*boxCollider = new ColliderBox(Name() + L"Box" + to_wstring(j), min, max, lineEffect);
				boxCollider->Ready();
				boxCollider->Name() = name;

				weapon->GetColliders().push_back(boxCollider);
			}
			else if (typeName == L"Sphere")
			{
				D3DXVECTOR3 centerPos = Math::GetCenterPos(min, max);

				float radius = (min.x - max.x) / 2.0f;

				ColliderSphere*sphereCollider = new ColliderSphere(Name() + L"Sphere" + to_wstring(j), centerPos, radius, lineEffect);
				sphereCollider->Ready();
				sphereCollider->Name() = name;

				weapon->GetColliders().push_back(sphereCollider);
			}
			else if (typeName == L"Ray")
			{
				D3DXVECTOR3 centerPos = Math::GetCenterPos(min, max);

				ColliderRay*rayCollider = new ColliderRay(Name() + L"Ray" + to_wstring(colliders.size()), centerPos, Direction(), lineEffect);
				rayCollider->Ready();
				rayCollider->Name() = name;

				weapon->GetColliders().push_back(rayCollider);
			}

			D3DXVECTOR3 position, scale, rotate;
			float distance;
			D3DXVECTOR3 dir;

			col = col->NextSiblingElement(); // ColliderPosX
			position.x = col->FloatText();

			col = col->NextSiblingElement(); // ColliderPosY
			position.y = col->FloatText();

			col = col->NextSiblingElement(); // ColliderPosZ
			position.z = col->FloatText();

			col = col->NextSiblingElement(); // ColliderScaleX
			scale.x = col->FloatText();

			col = col->NextSiblingElement(); // ColliderScaleY
			scale.y = col->FloatText();

			col = col->NextSiblingElement(); // ColliderScaleZ
			scale.z = col->FloatText();

			col = col->NextSiblingElement(); // ColliderRotateX
			rotate.x = col->FloatText();

			col = col->NextSiblingElement(); // ColliderRotateY
			rotate.y = col->FloatText();

			col = col->NextSiblingElement(); // ColliderRotateZ
			rotate.z = col->FloatText();

			weapon->GetColliders()[j]->Scale(scale);
			weapon->GetColliders()[j]->Rotation(rotate);
			weapon->GetColliders()[j]->Position(position);

			if (typeName == L"Ray")
			{
				ColliderRay*ray = dynamic_cast<ColliderRay*>(weapon->GetColliders()[j]);

				col = col->NextSiblingElement(); // Distance
				distance = col->FloatText();

				col = col->NextSiblingElement(); // DirectionX
				dir.x = col->FloatText();

				col = col->NextSiblingElement(); // DirectionY
				dir.y = col->FloatText();

				col = col->NextSiblingElement(); // DirectionZ
				dir.z = col->FloatText();

				ray->GetRay()->Distance = distance;
				ray->GetRay()->Direction = dir;
			}

			weaponElement = weaponElement->NextSiblingElement();
		}
		if(i!= weaponCount-1)
			node = node->NextSiblingElement();
		weapons.push_back(weapon);
	}
	BT->ReadBehaviorTree(node, BT->RootNode());

	Play();

	SAFE_DELETE(document);
}

void Player::ColliderUpdate()
{
	for (size_t i = 0; i < colliders.size(); i++)
	{
		/*CollisionResult * result = CollisionContext::Get()->HitTest(colliders[i], CollisionContext::Get()->GetLayer(L"Collision World"), CollisionResult::ResultType::ENearestOne);

		if (result != NULL)
			walkSpeed = -45;
		else
			walkSpeed = 10;*/


		CollisionResult * result2 = CollisionContext::Get()->HitTest(colliders[0], CollisionContext::Get()->GetLayer(L"Collision EnemyAttack"), CollisionResult::ResultType::ENearestOne);
		
		if (result2 != NULL)
		{
			if (hp > 0)
			{
				isDamaged = true;
				
				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 0, 0, 1));
				StartTargetClip(L" Damaged.anim");
			}
		}

		CollisionResult * result3 = CollisionContext::Get()->HitTest(colliders[0], CollisionContext::Get()->GetLayer(L"Collision EnemySkill1"), CollisionResult::ResultType::ENearestOne);

		if (result3 != NULL)
		{
			if (hp > 0&&boss->IsAttack2())
			{
				isDamaged = true;

				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1,0, 0, 1));
				StartTargetClip(L" Damaged.anim");
			}
		}

		CollisionResult * result4 = CollisionContext::Get()->HitTest(colliders[0], CollisionContext::Get()->GetLayer(L"Collision EnemySkill2"), CollisionResult::ResultType::ENearestOne);

		if (result3 != NULL)
		{
			if (hp > 0&&boss->IsAttack3())
			{
				isDamaged = true;

				InstanceManager::Get()->FindAnim(GetModel()->GetModelName())->ChangeDiffuse(instanceID, D3DXCOLOR(1, 0, 0, 1));
				StartTargetClip(L" Damaged.anim");
			}
		}

	}
}

D3DXVECTOR3 & Player::GetCamPosition()
{
	return camPos;
}

bool & Player::LoadStart()
{
	for (size_t i = 0; i < weapons.size(); i++)
		weapons[i]->LoadStart()=false;
	return isLoad;
}


bool Player::MoveTPS()
{
	static bool isonceMove = false;
	if (Keyboard::Get()->Press('W') ||
		Keyboard::Get()->Press('S') ||
		Keyboard::Get()->Press('A') ||
		Keyboard::Get()->Press('D'))
	{
		//앞으로 이동

		if (isDamaged)return false;
		if (hp <= 0) return false;

		if (!isonceMove)
		{
			soundManager::Get()->play("13Walk");
			isonceMove = true;
		}
		if (Keyboard::Get()->Press('W'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			if (Keyboard::Get()->Press(VK_SHIFT))return false;
			if (Keyboard::Get()->Down(VK_SPACE))return false;
			if (Mouse::Get()->Press(1))return false;
			
			if (GetPlayClip() == GetClip(L" Diving.anim"))return false;
			if (GetPlayClip() == GetClip(L" Reload.anim"))return false;
			
			if (Keyboard::Get()->Press('A'))
			{
				if(crossVec==D3DXVECTOR3(0,0,0))
					crossVec = -Right();

				walkSpeed = 9;
				StartTargetClip(L" ForwardWalkLeft.anim");
			}
			else if (Keyboard::Get()->Press('D'))
			{
				if (crossVec == D3DXVECTOR3(0, 0, 0))
					crossVec = Right();

				walkSpeed = 9;
				StartTargetClip(L" ForwardWalkRight.anim");
			}
			else
			{
				crossVec = { 0,0,0 };
				walkSpeed = 10;
				StartTargetClip(L" ForwardWalk.anim");
			}

			curPos += -Time::Delta()*(Direction()+ crossVec) * walkSpeed;
			Position(curPos);
		}
		//뒤로 이동
		else if (Keyboard::Get()->Press('S'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			if (Keyboard::Get()->Down(VK_SPACE))
			{
				soundManager::Get()->play("38Dash",0.6f);
				backDash = true;
			}

			if (backDash)
			{
				curPos += Time::Delta()*Direction() * 28 * gValue;
				Position(curPos);
				StartTargetClip(L" BackJump.anim");

				D3DXVECTOR3 p = GetPosition();
				p.y += 2.8f;
				ParticleManager::Get()->Shot(L"DashEffect", p);
				if (gValue>0)
					gValue -= Time::Delta();

				if (GetClip(L" BackJump.anim")->IsAnimEnd())
				{
					ParticleManager::Get()->UnShot(L"DashEffect");
					backDash = false;
					gValue = 1;
				}
			}
			else
			{
				if (Keyboard::Get()->Press('A'))
				{
					if (crossVec == D3DXVECTOR3(0, 0, 0))
						crossVec = Right();

					walkSpeed = 9;
					StartTargetClip(L" BackWalkLeft.anim");
				}
				else if (Keyboard::Get()->Press('D'))
				{
					if (crossVec == D3DXVECTOR3(0, 0, 0))
						crossVec = -Right();

					walkSpeed = 9;
					StartTargetClip(L" BackWalkRight.anim");
				}
				else
				{
					crossVec = { 0,0,0 };
					walkSpeed = 10;
					StartTargetClip(L" BackWalk.anim");
				}

				curPos += Time::Delta()*(Direction() + crossVec) * walkSpeed;
				Position(curPos);
			}
		}
		//왼쪽 이동
		else if (Keyboard::Get()->Press('A'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			if (Keyboard::Get()->Down(VK_SPACE))
			{
				soundManager::Get()->play("38Dash", 0.6f);
				leftDash = true;
			}

			if (leftDash)
			{
				curPos += Time::Delta()*Right() *28*gValue;
				Position(curPos);
				StartTargetClip(L" LeftJump.anim");

				D3DXVECTOR3 p = GetPosition();
				p.y += 2.8f;
				ParticleManager::Get()->Shot(L"DashEffect", p);
				if (gValue>0)
					gValue -= Time::Delta();

				if (GetClip(L" LeftJump.anim")->IsAnimEnd())
				{
					ParticleManager::Get()->UnShot(L"DashEffect");
					leftDash = false;
					gValue = 1;
				}
			}
			else
			{
				curPos += Time::Delta()*Right() *walkSpeed;
				Position(curPos);
				StartTargetClip(L" LeftWalk.anim");
			}
		}
		//오른쪽 이동
		else if (Keyboard::Get()->Press('D'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			if (Keyboard::Get()->Down(VK_SPACE))
			{
				soundManager::Get()->play("38Dash", 0.6f);
				rightDash = true;
			}

			if (rightDash)
			{
				curPos += -Time::Delta()*Right() * 28* gValue;
				Position(curPos);
				StartTargetClip(L" RightJump.anim");

				D3DXVECTOR3 p = GetPosition();
				p.y += 2.8f;
				ParticleManager::Get()->Shot(L"DashEffect", p);
				if(gValue>0)
					gValue -= Time::Delta();

				if (GetClip(L" RightJump.anim")->IsAnimEnd())
				{
					ParticleManager::Get()->UnShot(L"DashEffect");
					rightDash = false;
					gValue = 1;
				}
			}
			else
			{
				curPos += -Time::Delta()*Right() * walkSpeed;
				Position(curPos);
				StartTargetClip(L" RightWalk.anim");
			}
		}
		return true;
	}
	else
	{
		gValue = 1;
		leftDash = false;
		rightDash = false;
		backDash = false;
		isonceMove = false;
		soundManager::Get()->stop("13Walk");
		ParticleManager::Get()->UnShot(L"DashEffect");
		return false;
	}
}

void Player::MoveShotting()
{
	if (Keyboard::Get()->Press('W') ||
		Keyboard::Get()->Press('S') ||
		Keyboard::Get()->Press('A') ||
		Keyboard::Get()->Press('D'))
	{
		//앞으로 이동
		if (Keyboard::Get()->Press('W'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += -Time::Delta()*(Direction() + crossVec) * walkSpeed;
			Position(curPos);
		}
		//뒤로 이동
		else if (Keyboard::Get()->Press('S'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += Time::Delta()*(Direction() + crossVec) * (walkSpeed+4);
			Position(curPos);
		}
		//왼쪽 이동
		else if (Keyboard::Get()->Press('A'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += Time::Delta()*Right() *walkSpeed;
			Position(curPos);
		}
		//오른쪽 이동
		else if (Keyboard::Get()->Press('D'))
		{
			D3DXVECTOR3 curPos;
			Position(&curPos);

			curPos += -Time::Delta()*Right() * walkSpeed;
			Position(curPos);
		}
	}
}

void Player::Turn()
{
	if (GetPlayClip() == GetClip(L" Diving.anim")||
		GetPlayClip() == GetClip(L" Reload.anim"))return;

	D3DXVECTOR3 rotation;
	Rotation(&rotation);

	D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	//관전모드
	if (Mouse::Get()->Press(2))
	{
		if (terrain->GetHeight(camPos.x, camPos.z) + 0.5f <= camPos.y)
		{
			camRotate.x -= val.y * 0.2f * Time::Delta();
		}
		else
		{
			camRotate.x = camRotate.x + Math::ToRadian(-0.05f);
		}
		camRotate.y += val.x * 0.2f * Time::Delta();
	}
	else
	{
		if (terrain->GetHeight(camPos.x, camPos.z) + 0.5f <= camPos.y)
		{
			camRotate.x -= val.y * 0.2f * Time::Delta();
		}
		else
		{
			camRotate.x = camRotate.x + Math::ToRadian(-0.01f);
		}
		camRotate.y = rotation.y + val.x * 0.2f * Time::Delta();


		if (GetPlayClip() == GetClip(L" Shot.anim") || GetPlayClip() == GetClip(L" Aim.anim") || GetPlayClip() == GetClip(L" Shot2.anim"))
		{
			if (GetPlayClip() == GetClip(L" Shot.anim") || GetPlayClip() == GetClip(L" Shot2.anim"))
			{
				if (weaponState == RAYCASTGUN)
				{
					if (CurrentKeyFrame() > 5)
						camRotate.x += rebound - Time::Get()->Delta()*0.03f;
					else
						camRotate.x += rebound;
				}
				else if(weaponState==MUCHINGUN)
					camRotate.x += rebound;
			}

			Rotation(camRotate.x, camRotate.y, 0);
		}
		else
			Rotation(0, camRotate.y, 0);
	}
}

void Player::UpdateWorld()
{
	GameRender::UpdateWorld();

	InstanceManager::Get()->UpdateWorld(this, instanceID, World(), InstanceManager::InstanceType::ANIMATION);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(World());
	}
}


