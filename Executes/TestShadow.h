#pragma once
#include "Execute.h"

//   AlphaBlend : ������ �̹���
//	 BlendState�� ����
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