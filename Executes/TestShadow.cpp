#include "stdafx.h"
#include "../Executes/TestShadow.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimModel.h"
#include "../Tool/ToolManager.h"
#include "../Objects/MeshPlane.h"
#include "../DebugDraw\DebugCone.h"
#include "../Light/Shadow.h"
#include "../Viewer/Follow.h"


TestShadow::TestShadow(ExecuteValues * values)
	: Execute(values)
{
	kachujin = new GameAnimModel
	(
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh",
		values
	);

	Shader * shader = new Shader(Shaders + L"033_Animation.hlsl");
	kachujin->SetShader(shader);
	kachujin->SetSpecularMap(Models + L"Kachujin/Kachujin_specular.png");
	kachujin->SetNormalMap(Models + L"Kachujin/Kachujin_normal.png");
	kachujin->SetShininess(30);

	kachujin->AddClip(Models + L"Kachujin/Running.anim");
	kachujin->Play(0, true, 0, 0.1, 0);

	kachujin->Scale(0.025, 0.025, 0.025);


	plane = new MeshPlane(values);
	shader = new Shader(Shaders + L"042_Plane.hlsl");
	plane->SetShader(shader);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->SetDiffuseMap(Textures + L"Bricks.png");
	plane->Scale(10, 1, 10);

	tm = new ToolManager(values);

	shadow = new Shadow(values);
	shadow->Add(kachujin);
	shadow->Add(plane);
}

TestShadow::~TestShadow()
{
	//SAFE_DELETE(kachujin);

}

void TestShadow::Update()
{
	tm->Update();
	//shadow->Update();
	//kachujin->Update();
}

void TestShadow::PreRender()
{
	tm->PreRender();
	//shadow->PreRender();
}

void TestShadow::Render()
{
	tm->Render();
	//shadow->Render();
	//kachujin->Render();
}

void TestShadow::PostRender()
{
	tm->PostRender();
}

void TestShadow::ResizeScreen()
{
}

