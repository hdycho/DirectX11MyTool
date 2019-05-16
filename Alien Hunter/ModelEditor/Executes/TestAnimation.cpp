#include "stdafx.h"
#include "TestAnimation.h"
#include "Fbx/FbxLoader.h"
#include "Model\Model.h"

void TestAnimation::Initialize()
{
	/*FbxLoader * loader = NULL;

	loader = new FbxLoader
	(
	Assets + L"Player/ganfaul_m_aure.fbx",
	Models + L"Player/", L"Player"
	);
	loader->ExportMaterial();
	loader->ExportMesh();
	SAFE_DELETE(loader);*/

	for(int i=0; i<1; i++)
		model[i] = new GameAnimator(i, Models + L"Player/Player.material", Models + L"Player/Player.mesh");

}

void TestAnimation::Ready()
{
	//clip = new ModelClip(Models + L"Player/Slash.anim");

	for (int i = 0; i < 1; i++)
	{
		model[i]->Ready();
		model[i]->AddClip(Models + L"Player/Slash.anim",true);
		model[i]->AddClip(Models + L"Player/Run.anim", true);
		model[i]->AddClip(Models + L"Player/Slash2.anim", true);
		model[i]->AddClip(Models + L"Player/Slash3.anim", true);
		model[i]->AddClip(Models + L"Player/Idle.anim", true);
	}

	model[0]->Position(-10, 0, 0);
	model[0]->Play();
}

void TestAnimation::Destroy()
{
	//SAFE_DELETE(model);
	//SAFE_DELETE(clip);
}

void TestAnimation::Update()
{
	if (Keyboard::Get()->Down('P'))
	{
		model[0]->StartTargetClip(L" Run.anim");
	}
	if (Keyboard::Get()->Down('O'))
	{
		model[0]->StartTargetClip(L" Slash.anim");
	}
	model[0]->Update();
}

void TestAnimation::PreRender()
{

}

void TestAnimation::Render()
{
	
}

void TestAnimation::PostRender()
{
}

void TestAnimation::ResizeScreen()
{
}

void TestAnimation::TestImgui()
{
}
