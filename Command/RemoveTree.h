#pragma once
#include "Command.h"
class RemoveTree :public Command
{
public:
	void Execute(class TreesManager*tm, void * data) override;
};
