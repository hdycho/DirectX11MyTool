#include "stdafx.h"
#include "../Executes/TestProjection.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimModel.h"
#include "../Tool/ToolManager.h"
#include "../Objects/MeshPlane.h"
#include "../DebugDraw\DebugCone.h"
#include "../Light/Projection.h"


TestProjection::TestProjection(ExecuteValues * values)
	: Execute(values)
{
	//Fbx::Exporter*exporter = NULL;
	//Kachuzin
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Mesh.fbx");
	//exporter->ExportMaterial(Models + L"Kachujin/", L"Kachujin.material");
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Samba_Dancing.fbx");
	//exporter->ExportAnimation(Models + L"Kachujin/", L"Samba_Dance.anim", 0);
	//SAFE_DELETE(exporter);

	kachujin = new GameAnimModel
	(
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh",
		values
	);

	Shader * shader = new Shader(Shaders + L"033_Animation.hlsl");
	kachujin->SetShader(shader);
	kachujin->SetShininess(30);
	kachujin->SetSpecularMap(Models + L"Kachujin/Kachujin_specular.png");
	kachujin->SetNormalMap(Models + L"Kachujin/Kachujin_normal.png");

	kachujin->AddClip(Models + L"Kachujin/Running.anim");
	kachujin->Play(0, true, 0, 15, 0);

	kachujin->Scale(0.025, 0.025, 0.025);


	plane = new MeshPlane(values);
	shader = new Shader(Shaders + L"042_Plane.hlsl");
	plane->SetShader(shader);
	plane->Scale(10, 1, 10);
	plane->SetDiffuse(0, 0, 0, 1);

	//tm = new ToolManager(values);

	projection = new Projection(values);
	projection->Add(kachujin);
	projection->Add(plane);
}

TestProjection::~TestProjection()
{
	SAFE_DELETE(kachujin);

}

void TestProjection::Update()
{
	projection->Update();
	//kachujin->Update();
}

void TestProjection::PreRender()
{

}

void TestProjection::Render()
{
	projection->Render();
	//kachujin->Render();
	//plane->Render();
	//tm->Render();
}

void TestProjection::PostRender()
{

}

void TestProjection::ResizeScreen()
{
}

