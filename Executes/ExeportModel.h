#pragma once
#include "Execute.h"

class ExeportModel : public Execute
{
private:
	typedef VertexTextureNormal VertexType;

public:
	ExeportModel(ExecuteValues* values);
	~ExeportModel();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:
};


