#pragma once
#include "Command.h"

class UnClickTree :public Command
{
public:
	void Execute(class TreesManager*tm, void * data) override;
};

