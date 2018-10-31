#pragma once

class ICollision
{
public:
	virtual bool OnCollisionEnter() = 0;
	virtual bool OnCollisionExit() = 0;
};