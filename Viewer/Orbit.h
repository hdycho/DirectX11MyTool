#pragma once
#include "Camera.h"

class Orbit :public Camera
{

public:
	Orbit(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Orbit();

	void InputInfo(D3DXVECTOR3 startPos);
	void Update();
	float&GetSetZ() { return moveValue; }
	D3DXVECTOR3&GetTargetPos() { return startPos; }
	D3DXVECTOR2&GetRotate() { return rotateValue; }
private:
	float moveSpeed;
	float rotationSpeed;
	D3DXVECTOR3 startPos;
	D3DXVECTOR3*moveStartPos;
	D3DXVECTOR3*moveRotate;
	D3DXVECTOR2 rotateValue;
	float moveValue = 20;
};