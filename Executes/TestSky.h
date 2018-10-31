#pragma once
#include "Execute.h"

//   AlphaBlend : 한장의 이미지
//	 BlendState로 조절
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