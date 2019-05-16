#include "stdafx.h"
#include "TestTool.h"
#include "Fbx/FbxLoader.h"
#include "Tools\ObjectTool.h"

void TestTool::Initialize()
{
	tool = new ObjectTool();
	
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
	//tool->PostRender();
}

void TestTool::ResizeScreen()
{
}
