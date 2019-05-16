#include "stdafx.h"
#include "TestTool.h"
#include "Fbx/FbxLoader.h"
#include "Tools\SceneTool.h"
#include "Environment\ParticleSystem.h"

void TestTool::Initialize()
{
	//tool = new SceneTool();
}

void TestTool::Ready()
{
	//tool->Ready();

	vector<wstring> names;
	for (int i = 1; i <= 1; i++)
	{
		names.push_back(Textures + L"GameScene/Smoke.png");
	}
	ps = new ParticleSystem(names,1);
	ps->Initialize(15, 7, D3DXVECTOR2(2, 2), 451, 0.645f, 0.1f, 300);
	ps->Ready();


	/*ship = new GameModel(0, Models + L"Player/Player.material", Models + L"Player/Player.mesh");
	ship->Ready();
	ship->Position(0, 0, 0);
	ship->Scale(2.5, 2.5, 2.5);*/
}

void TestTool::Destroy()
{
	//SAFE_DELETE(tool);
	SAFE_DELETE(ps);
}

void TestTool::Update()
{
	//tool->Update();
	//ship->Update();
	//ParticleManager::Get()->Shot(L"Fire");

	ps->KillParticle();
	ps->EmitParticles();
}

void TestTool::PreRender()
{
	//tool->PreRender();
}

void TestTool::Render()
{
	//tool->Render();
	InstanceManager::Get()->Render();

	ps->Render();
	ps->ImGuiRender();
}

void TestTool::PostRender()
{
	//tool->PostRender();
}

void TestTool::ResizeScreen()
{
}
