#pragma once
#include "Execute.h"

//   AlphaBlend : 한장의 이미지
//	 BlendState로 조절
class TestShadow : public Execute
{
public:
	TestShadow(ExecuteValues* values);
	~TestShadow();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class ToolManager*tm;
	class GameAnimModel*kachujin;
	class MeshPlane*plane;

	class Shadow*shadow;
};