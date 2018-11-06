#include "stdafx.h"
#include "Player.h"
#include "../Model/ModelClip.h"
#include "../Viewer/TPSCamera.h"
#include "../Physics/LineMaker.h"
#include "../LandScape/TerrainRender.h"

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, ExecuteValues * values)
	:GameAnimModel(matFolder,matFile,meshFolder,meshFile,values)
{
	myPtr = this;
	className = L"Player";

	pState = PLAYERSTATE::NONE;

	target = new Texture(Textures + L"scope_target.png");
	render = new Render2D(values);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	render->Scale(desc.Width*0.05, desc.Height*0.08);
	render->Position(desc.Width/2-35, desc.Height/2);

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->DestBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendMode[1]->BlendOp(D3D11_BLEND_OP_ADD);
	saveTargetY = 0;
	cam = NULL;
	terrain = NULL;

	shotLine = new LineMaker();
	shotLine->SizeInit(1000);
	isShot = false;
}

Player::Player(Player & player)
	:GameAnimModel(player)
{
	myPtr = this;
	className = L"Player";

	pState = PLAYERSTATE::NONE;

	target = player.target;
	render = player.render;

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->DestBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendMode[1]->BlendOp(D3D11_BLEND_OP_ADD);
	saveTargetY = 0;
	cam = NULL;
	terrain = NULL;

	shotLine = new LineMaker();
	shotLine->SizeInit(1000);
	shotStart = { 0,0,0 };
	isShot = false;
}

Player::~Player()
{
	SAFE_DELETE(render);
	SAFE_DELETE(target);
	SAFE_DELETE(blendMode[0]);
	SAFE_DELETE(blendMode[1]);
}

bool Player::OnCollisionEnter()
{
	SetDiffuse(1, 0, 0, 1);
	return true;
}

bool Player::OnCollisionExit()
{
	SetDiffuse(1, 1, 1, 1);
	return true;
}

void Player::Update()
{
	__super::Update();
	D3DXVECTOR3 pos(0, 100+50, 0);

	D3DXVec3TransformCoord(&pos, &pos, &World());
	D3DXVECTOR3 start = pos;
	shotStart = start;

	D3DXVECTOR3 dir = { Direction().x,Direction().y+ 0.17f,Direction().z};
	dir *= -1;
	shotDir = dir;

	shotLine->UpdateLine(start, dir, 1000);
	
	if (terrain != NULL)
	{
		D3DXVECTOR3 curPos = Position();
		float adjustY = terrain->Y(curPos);
		curPos.y = adjustY;
		Position(curPos);
	}
}

void Player::FixedUpdate()
{
	switch (pState)
	{
		case Player::PLAYERSTATE::NONE:
		{
			pState = PLAYERSTATE::IDLE;
			Play((int)PLAYERSTATE::IDLE, true,0, 20,0);
			isShot = false;
		}
		break;
		case Player::PLAYERSTATE::IDLE:
		{
			isShot = false;
			if (IsMoveStay())
			{
				pState = PLAYERSTATE::MOVE;
				Play((int)PLAYERSTATE::MOVE, true, 10,20,0);
			}

			if (Mouse::Get()->Press(1))
			{
				pState = PLAYERSTATE::AIM;
				Play((int)PLAYERSTATE::AIM, true, 10, 20, 0);
				cam->UseToZoom(true, D3DXVECTOR3(0.95f,-0.95f,7.75f));
			}
			
			Turn();
		}
		break;
		case Player::PLAYERSTATE::MOVE:
		{
			/*if (Move() == false)
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true,10, 20, 0);
			}*/

			if (MoveTPS() == false)
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true,10, 20, 0);
			}
			Turn();
		}
		break;
		case Player::PLAYERSTATE::AIM:
		{
			if (Mouse::Get()->Up(1))
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true, 10, 20, 0);
				cam->UseToZoom(false, D3DXVECTOR3(0,0,0));
			}

			if (Mouse::Get()->Down(0))
			{
				shotLine->SetColor(D3DXCOLOR(0, 0, 1, 1));
				pState = PLAYERSTATE::ATTACK;
				Play((int)PLAYERSTATE::ATTACK, false, 10, 20, 0);
				GetAnimClip()[(int)PLAYERSTATE::ATTACK]->PlayState() = true;
				isShot = true;
			}

			Turn();
		}
		break;
		case Player::PLAYERSTATE::ATTACK:
		{
			if (!GetAnimClip()[(int)PLAYERSTATE::ATTACK]->PlayState())
			{
				shotLine->SetColor(D3DXCOLOR(1, 0, 0, 1));
				pState = PLAYERSTATE::AIM;
				Play((int)PLAYERSTATE::AIM, true, 10, 20, 0);
				isShot = false;
			}
		}
		break;
	}
}

void Player::Render()
{
	__super::Render();

	if (pState == Player::PLAYERSTATE::AIM || pState == Player::PLAYERSTATE::ATTACK)
	{
		shotLine->DrawLine();
	}
}

void Player::PostRender()
{
	if (pState == Player::PLAYERSTATE::AIM || pState == Player::PLAYERSTATE::ATTACK)
	{
		blendMode[1]->OMSetBlendState();
		render->Update();
		render->SRV(target->GetView());
		render->Render();
		blendMode[0]->OMSetBlendState();
	}
}

bool Player::Move()
{
	D3DXVECTOR3 vec(0, 0, 0);
	D3DXVECTOR3 velocity(0, 0, 0);
	float time = 0;
	if (Keyboard::Get()->Press(VK_UP) || Keyboard::Get()->Press(VK_DOWN) ||
		Keyboard::Get()->Press(VK_LEFT) || Keyboard::Get()->Press(VK_RIGHT))
	{
		if (Keyboard::Get()->Press(VK_LEFT))
		{
			velocity.x = -7;
			vec += D3DXVECTOR3(1, 0, 0)*velocity.x;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);

			D3DXVECTOR3 rotate;

			D3DXQUATERNION start, end, result;

			//시작점
			D3DXQuaternionRotationYawPitchRoll(&start, Rotation().y, 0, 0);

			//끝날지점
			D3DXQuaternionRotationYawPitchRoll(&end, Math::ToRadian(90), 0, 0);

			time += Time::Delta() * 5.0f;
			D3DXQuaternionSlerp(&result, &start, &end, time);

			Math::toEulerAngle(result, rotate);

			Rotation(0, rotate.y, 0);
		}
		if (Keyboard::Get()->Press(VK_RIGHT))
		{
			velocity.x = 7;
			vec += D3DXVECTOR3(1, 0, 0)*velocity.x;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);

			D3DXVECTOR3 rotate;

			D3DXQUATERNION start, end, result;

			//시작점
			D3DXQuaternionRotationYawPitchRoll(&start, Rotation().y, 0, 0);

			//끝날지점
			D3DXQuaternionRotationYawPitchRoll(&end, Math::ToRadian(270), 0, 0);

			time += Time::Delta() * 5.0f;
			D3DXQuaternionSlerp(&result, &start, &end, time);

			Math::toEulerAngle(result, rotate);

			Rotation(0, rotate.y, 0);
		}
		if (Keyboard::Get()->Press(VK_UP))
		{
			velocity.z = 7;
			vec += D3DXVECTOR3(0, 0, 1)*velocity.z;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);

			D3DXVECTOR3 rotate;

			D3DXQUATERNION start, end, result;

			//시작점
			D3DXQuaternionRotationYawPitchRoll(&start, Rotation().y, 0, 0);

			//끝날지점
			D3DXQuaternionRotationYawPitchRoll(&end, Math::ToRadian(180), 0, 0);

			time += Time::Delta() * 5.0f;
			D3DXQuaternionSlerp(&result, &start, &end, time);

			Math::toEulerAngle(result, rotate);

			Rotation(0, rotate.y, 0);
		}
		if (Keyboard::Get()->Press(VK_DOWN))
		{
			velocity.z = -7;
			vec += D3DXVECTOR3(0, 0, 1)*velocity.z;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);

			D3DXVECTOR3 rotate;

			D3DXQUATERNION start, end, result;

			//시작점
			D3DXQuaternionRotationYawPitchRoll(&start, Rotation().y, 0, 0);

			//끝날지점
			D3DXQuaternionRotationYawPitchRoll(&end, Math::ToRadian(360), 0, 0);

			time += Time::Delta() * 5.0f;
			D3DXQuaternionSlerp(&result, &start, &end, time);

			Math::toEulerAngle(result, rotate);

			Rotation(0, rotate.y, 0);
		}
		return true;
	}

	return false;
}

bool Player::MoveDirect()
{
	D3DXVECTOR3 vec(0, 0, 0);
	D3DXVECTOR3 velocity(0, 0, 0);
	D3DXVECTOR3 rotation(0, 0, 0);
	float time = 0;
	if (Keyboard::Get()->Press(VK_UP) ||Keyboard::Get()->Press(VK_LEFT) || 
		Keyboard::Get()->Press(VK_RIGHT))
	{
		if (Keyboard::Get()->Press(VK_UP))
		{
			//앞으로 이동하기
			velocity.z = -7;
			vec +=Direction()*velocity.z;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);
		}

		//회전코드
		if (Keyboard::Get()->Press(VK_LEFT))
		{
			//앞으로 이동하기
			rotation.y -= Math::ToRadian(90)*Time::Delta();

			D3DXVECTOR3 rotate = Rotation() + rotation;

			RotationDegree(rotate);
		}

		if (Keyboard::Get()->Press(VK_RIGHT))
		{
			rotation.y += Math::ToRadian(90)*Time::Delta();

			D3DXVECTOR3 rotate = Rotation() + rotation;

			RotationDegree(rotate);
		}
		
		
		return true;
	}
	return false;
}

bool Player::MoveTPS()
{
	D3DXVECTOR3 vec(0, 0, 0);
	D3DXVECTOR3 velocity(0, 0, 0);

	if (IsMoveStay())
	{
		if (Keyboard::Get()->Press('W'))
		{
			//앞으로 이동하기
			velocity.z = -7;
			vec += Direction()*velocity.z;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);
		}

		if (Keyboard::Get()->Press('S'))
		{
			//앞으로 이동하기
			velocity.z = 7;
			vec += Direction()*velocity.z;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);
		}

		if (Keyboard::Get()->Press('A'))
		{
			//앞으로 이동하기
			velocity.x = -7;
			vec += Right()*velocity.x;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);
		}

		if (Keyboard::Get()->Press('D'))
		{
			//앞으로 이동하기
			velocity.x = 7;
			vec += Right()*velocity.x;

			D3DXVECTOR3 pos = Position() + vec*Time::Delta();
			Position(pos);
		}

		return true;
	}
	return false;
}

void Player::Turn()
{
	D3DXVECTOR2 rotation = Rotation();

	D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

	rotation.y += val.x * 0.8f * Time::Delta();
	rotation.x -= val.y * 0.8f * Time::Delta();

	if (pState == Player::PLAYERSTATE::AIM|| pState == Player::PLAYERSTATE::ATTACK)
	{
		RotationDegree(rotation.x, rotation.y, 0);
		cam->GetChangeXvalue() = 0;
	}
	else
	{
		RotationDegree(0, rotation.y, 0);
		cam->GetChangeXvalue() += val.y * 0.8f * Time::Delta();
	}
}

bool Player::IsMoveStay()
{
	if (Keyboard::Get()->Press('W') || Keyboard::Get()->Press('A') ||
		Keyboard::Get()->Press('S') || Keyboard::Get()->Press('D'))
	{
		return true;
	}
	return false;
}

bool Player::IsMoveOnce()
{
	if (Keyboard::Get()->Down('W') || Keyboard::Get()->Down('A') ||
		Keyboard::Get()->Down('S') || Keyboard::Get()->Down('D'))
	{
		return true;
	}
	return false;
}
