#include "stdafx.h"
#include "../LandScape/Sky.h"
#include "../LandScape/TerrainRender.h"
#include "../Executes/TestSky.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimModel.h"
#include "../Tool/ToolManager.h"
#include "../Objects/MeshPlane.h"
#include "../DebugDraw\DebugCone.h"
#include "../Light/Shadow.h"
#include "../Viewer/Orbit.h"


TestSky::TestSky(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);
	tm = new ToolManager(values);
	//terrain = new TerrainRender(values);
}

TestSky::~TestSky()
{
	SAFE_DELETE(sky);
	//SAFE_DELETE(terrain);
}

void TestSky::Update()
{
	sky->Update();
	tm->Update();
	//terrain->Update();
}

void TestSky::PreRender()
{
	sky->PreRender();
	tm->PreRender();
}

void TestSky::Render()
{
	sky->Render();
	tm->Render();
	//terrain->Render();
}

void TestSky::PostRender()
{
	tm->PostRender();
}

void TestSky::ResizeScreen()
{
}



