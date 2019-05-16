#include "stdafx.h"
#include "TestTool.h"
#include "Tools\MapTool.h"

void TestTool::Initialize()
{
	tool = new MapTool();
}

void TestTool::Ready()
{
	tool->Ready();
}

void TestTool::Destroy()
{
	SAFE_DELETE(tool);
}

void TestTool::Update()
{
	tool->Update();
}

void TestTool::PreRender()
{
	tool->PreRender();
}

void TestTool::Render()
{
	tool->Render();
}

void TestTool::PostRender()
{
}

void TestTool::ResizeScreen()
{
}
