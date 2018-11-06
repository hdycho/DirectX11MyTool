#include "stdafx.h"
#include "TPSCamera.h"

TPSCamera::TPSCamera(float moveSpeed, float rotationSpeed)
	:moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
	rotateValue = { 0,0 };
	changeXvalue = 0;
	setY = 0;
	zoomValue = { 0,0,0 };
	isZoom = false;
	startZoom ={ 0,0,0 };
}

TPSCamera::~TPSCamera()
{
}

void TPSCamera::InputInfo(D3DXVECTOR3 * startPos, D3DXVECTOR3 * rotation)
{
	moveStartPos = startPos;
	moveRotate = rotation;
}

void TPSCamera::Update()
{
	//ȸ��ó��
	{
		Rotation(-moveRotate->x + rotateValue.x+ changeXvalue, moveRotate->y + Math::ToRadian(180));
	}

	//���� �����̴� �̵�*ȸ��*��ü�� ��ġ
	{
		D3DXMATRIX trans, controlRotate, targetTrans;
		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&controlRotate);
		D3DXMatrixIdentity(&targetTrans);

		if (isZoom)
		{
			if (startZoom.x < zoomValue.x)
				startZoom.x += 25*Time::Get()->Delta();

			if (startZoom.y < zoomValue.y)
				startZoom.y += 25*Time::Get()->Delta();

			if (startZoom.z < zoomValue.z)
				startZoom.z += 25*Time::Get()->Delta();
		}
		else
		{
			startZoom = zoomValue;
		}

		float mouseMiddleButton = Mouse::Get()->GetMoveValue().z*0.01f;
		moveValue += moveSpeed*mouseMiddleButton;

		D3DXMatrixTranslation(&trans, startZoom.x, startZoom.y, startZoom.z-moveValue);
		D3DXMatrixTranslation(&targetTrans, moveStartPos->x, moveStartPos->y + setY, moveStartPos->z);
		controlRotate = GetRotationMat();

		D3DXMATRIX result = trans*controlRotate*targetTrans;

		Position(result._41 , result._42 , result._43);
	}
}

void TPSCamera::UseToZoom(bool isZoom, D3DXVECTOR3 pos)
{
	this->isZoom=isZoom;
	zoomValue = pos;
}
