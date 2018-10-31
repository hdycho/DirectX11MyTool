#include "stdafx.h"
#include "Player.h"
#include "../Model/ModelClip.h"

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, ExecuteValues * values)
	:GameAnimModel(matFolder,matFile,meshFolder,meshFile,values)
{
	myPtr = this;
	className = L"Player";

	pState = PLAYERSTATE::NONE;
}

Player::Player(Player & player)
	:GameAnimModel(player)
{
	myPtr = this;
	className = L"Player";

	pState = PLAYERSTATE::NONE;
}

Player::~Player()
{

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

}

void Player::FixedUpdate()
{
	switch (pState)
	{
		case Player::PLAYERSTATE::NONE:
		{
			pState = PLAYERSTATE::IDLE;
			Play((int)PLAYERSTATE::IDLE, true,0, 20,0);
		}
		break;
		case Player::PLAYERSTATE::IDLE:
		{
			if (Keyboard::Get()->Down(VK_UP) || Keyboard::Get()->Down(VK_DOWN) ||
				Keyboard::Get()->Down(VK_LEFT) || Keyboard::Get()->Down(VK_RIGHT))
			{
				pState = PLAYERSTATE::MOVE;
				Play((int)PLAYERSTATE::MOVE, true, 10,20,0);
			}
			
			if (Mouse::Get()->Down(0))
			{
				pState = PLAYERSTATE::ATTACK;
				Play((int)PLAYERSTATE::ATTACK, false, 10, 20, 0);
				GetAnimClip()[(int)PLAYERSTATE::ATTACK]->PlayState() = true;

			}
		}
		break;
		case Player::PLAYERSTATE::MOVE:
		{
			/*if (Move() == false)
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true,10, 20, 0);
			}*/

			if (MoveDirect() == false)
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true,10, 20, 0);
			}
		}
		break;
		case Player::PLAYERSTATE::ATTACK:
		{
			if (!GetAnimClip()[(int)PLAYERSTATE::ATTACK]->PlayState())
			{
				pState = PLAYERSTATE::IDLE;
				Play((int)PLAYERSTATE::IDLE, true, 10, 20, 0);
			}
		}
		break;
	}
}

void Player::Render()
{
	__super::Render();


}

void Player::PostRender()
{

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
