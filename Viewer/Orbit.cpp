#include "stdafx.h"
#include "Orbit.h"

Orbit::Orbit(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
	rotateValue = { 0,0 };
}

Orbit::~Orbit()
{
}

void Orbit::InputInfo(D3DXVECTOR3 startPos)
{
	this->startPos = startPos;
}

void Orbit::Update()
{	
	//ȸ��ó��
	{
		if (Keyboard::Get()->Press('A'))
			rotateValue.y += rotationSpeed*Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			rotateValue.y -= rotationSpeed*Time::Delta();

		if (Keyboard::Get()->Press('W'))
			rotateValue.x += rotationSpeed*Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			rotateValue.x -= rotationSpeed*Time::Delta();

		Rotation(rotateValue.x, rotateValue.y);
	}
	//�̵�ó��
	{
		//ī�޶���� => �̵� => ȸ�� => ������ ��ǥ�� �̵�
		D3DXMATRIX trans, rotate, targetTrans;
		D3DXVECTOR3 resultPos(0, 0, 0);
		D3DXMATRIX result;

		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&targetTrans);
		D3DXMatrixIdentity(&result);

		if (Keyboard::Get()->Press('Y'))
			moveValue += moveSpeed*Time::Delta();
		else if (Keyboard::Get()->Press('H'))
			moveValue -= moveSpeed*Time::Delta();

		D3DXMatrixTranslation(&trans, 0, 0, moveValue);
		D3DXMatrixTranslation(&targetTrans, startPos.x, startPos.y, startPos.z);
		rotate = GetRotationMat();

		result = trans*rotate;
		D3DXVec3TransformCoord(&resultPos, &resultPos, &result);
		D3DXVec3TransformCoord(&resultPos, &resultPos, &targetTrans);

		Position(resultPos.x, resultPos.y, resultPos.z);
	}
}