#pragma once
#include "Execute.h"

//   AlphaBlend : ������ �̹���
//	BlendState�� ����
class TestProjection : public Execute
{
public:
	TestProjection(ExecuteValues* values);
	~TestProjection();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class ToolManager*tm;
	class GameAnimModel*kachujin;
	class MeshPlane*plane;

	class Projection*projection;
};