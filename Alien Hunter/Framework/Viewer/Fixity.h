#pragma once
#include "Camera.h" // 상속이나 특별한 경우만 헤더에 include 하자

class Fixity : public Camera
{
public:
	Fixity();
	~Fixity();

	void Update();

private:

};

