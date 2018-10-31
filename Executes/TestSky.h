#pragma once
#include "Execute.h"

//   AlphaBlend : ������ �̹���
//	 BlendState�� ����
class TestSky : public Execute
{
public:
	TestSky(ExecuteValues* values);
	~TestSky();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
private:
	class Sky*sky;
	//class TerrainRender*terrain;
	class ToolManager*tm;
};