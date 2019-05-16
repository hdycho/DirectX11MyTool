#pragma once
#include "Camera.h"

class TPSCamera :public Camera
{
public:
	TPSCamera(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~TPSCamera();

	void InputInfo(D3DXVECTOR3*startPos, D3DXVECTOR3*rotation);
	void Update();
	float&GetSetY() { return setY; }
	float&GetSetZ() { return moveValue; }
	D3DXVECTOR2&GetRotate() { return rotateValue; }
	float&GetChangeXvalue() { return changeXvalue; }

	void UseToZoom(bool isZoom, D3DXVECTOR3 pos);
	D3DXVECTOR3&GetStartPos() { return startZoom; }
private:
	float moveSpeed;
	float rotationSpeed;
	D3DXVECTOR3 startPos;
	D3DXVECTOR3*moveStartPos;
	D3DXVECTOR3*moveRotate;
	D3DXVECTOR2 rotateValue;
	float moveValue = 20;

	//y����ð�
	float setY;

	//�ܺο��� ����ȸ����Ű��X�ຯȭ��
	float changeXvalue;

	D3DXVECTOR3 zoomValue;
	D3DXVECTOR3 startZoom;
	D3DXVECTOR3 resultZoom;

	bool isZoom;
};