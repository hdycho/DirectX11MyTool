#pragma once
#include "Command.h"
class CreateTree:public Command
{
private:
	D3DXVECTOR3 pos;
public:
	CreateTree() {}
	CreateTree(D3DXVECTOR3 pos)
	{
		this->pos = pos;
	}

	void Execute(class TreesManager*tm, void * data) override;
};