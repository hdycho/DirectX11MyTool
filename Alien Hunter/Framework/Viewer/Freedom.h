#pragma once
#include "Camera.h" // ����̳� Ư���� ��츸 ����� include ����

class Freedom : public Camera
{
public:
	Freedom(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Freedom();

	void Update();

	void MoveSpeed(float val) { moveSpeed = val; }
	void RotationSpeed(float val) { rotationSpeed = val; }

private:
	float moveSpeed;
	float rotationSpeed;
};

