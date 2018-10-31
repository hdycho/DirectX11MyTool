#pragma once

class Command
{
public:
	virtual void Execute(class TreesManager*tm, void * data) = 0;
	void*&GetData() { return Data; }
	enum COMMANDTYPE ctype;
protected:
	void*Data;
};