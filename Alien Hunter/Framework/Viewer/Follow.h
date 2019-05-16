#pragma once
#include "Camera.h"

class Follow :public Camera
{
public:
	Follow(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Follow();

	void InputInfo(D3DXVECTOR3*startPos, D3DXVECTOR3*rotation);
	void Update();
	float&GetSetY() { return setY; }
	float&GetSetZ() { return moveValue; }
	D3DXVECTOR2&GetRotate() { return rotateValue; }
private:
	float moveSpeed;
	float rotationSpeed;
	D3DXVECTOR3 startPos;
	D3DXVECTOR3*moveStartPos;
	D3DXVECTOR3*moveRotate;
	D3DXVECTOR2 rotateValue;
	float moveValue = 20;

	//y축셋팅값
	float setY;
};