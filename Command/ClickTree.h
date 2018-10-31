#pragma once
#include "Command.h"

class ClickTree:public Command
{
public:
	void Execute(class TreesManager*tm, void * data) override;
};
