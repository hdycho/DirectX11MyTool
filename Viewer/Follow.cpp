#include "stdafx.h"
#include "Follow.h"

Follow::Follow(float moveSpeed, float rotationSpeed)
	:moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
	rotateValue = { 0,0 };
	setY = 0;
}

Follow::~Follow()
{
}

void Follow::InputInfo(D3DXVECTOR3 * startPos, D3DXVECTOR3 * rotation)
{
	moveStartPos = startPos;
	moveRotate = rotation;
}

void Follow::Update()
{
	//회전처리
	{
		/*if (Keyboard::Get()->Press('A'))
			rotateValue.x += rotationSpeed*Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			rotateValue.x -= rotationSpeed*Time::Delta();*/

		Rotation(moveRotate->x + rotateValue.x, moveRotate->y + Math::ToRadian(180));
	}

	//내가 움직이는 이동*회전*물체의 위치
	{
		D3DXMATRIX trans, controlRotate, targetTrans;
		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&controlRotate);
		D3DXMatrixIdentity(&targetTrans);

		if (Keyboard::Get()->Press('Y'))
			moveValue += moveSpeed*Time::Delta();
		else if (Keyboard::Get()->Press('H'))
			moveValue -= moveSpeed*Time::Delta();

		D3DXMatrixTranslation(&trans, 0, 0, -moveValue);
		D3DXMatrixTranslation(&targetTrans, moveStartPos->x, moveStartPos->y + setY, moveStartPos->z);
		controlRotate = GetRotationMat();

		D3DXMATRIX result = trans*controlRotate*targetTrans;

		Position(result._41, result._42, result._43);
	}
}
