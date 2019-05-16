#include "stdafx.h"
#include "LoadingScene.h"
#include "Viewer\Freedom.h"
#include "Environment\Sky.h"
#include "Environment\Terrain.h"
#include "Environment\Billboard.h"
#include "Environment\Rain.h"
#include "Environment\Rain.h"
#include "Environment\Shadow.h"
#include "Systems\Window.h"

void LoadingScene::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(0, 0);
	Context::Get()->GetMainCamera()->Position(0, 0, 0);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);


	Window::UseGameMouse();
	mainCam = Context::Get()->GetMainCamera();

	float width = Context::Get()->GetViewport()->GetWidth();
	float height = Context::Get()->GetViewport()->GetHeight();

	vector<wstring> names;

	for (int i = 0; i < 60; i++)
		names.push_back(Textures + L"GameScene/loadingImage/layer (" + to_wstring(i + 1) + L").png");

	textures = new TextureArray(names);
	render2d = new Render2D(L"022_2Danimation.hlsl");
	render2d->SRV(textures->GetSRV());
	render2d->Position(0, 0);
	render2d->Scale(1600, 900);
	frameIdx = 0;
	render2d->GetEffect()->AsScalar("frameIdx")->SetInt(frameIdx);
}

void LoadingScene::Ready()
{

}

void LoadingScene::Destroy()
{
	Context::Get()->GetMainCamera() = mainCam;

	SAFE_DELETE(render2d);
}

void LoadingScene::Update()
{
	ShowCursor(false);
	ShowCursor(false);

	if (frameIdx >= 60)
		frameIdx = 0;

	static float time = 0;
	static float checkTime = 0;
	time += Time::Get()->Delta();
	checkTime += Time::Get()->Delta();

	if (time > 0.03f)
	{
		frameIdx++;
		time = 0;
	}
	render2d->GetEffect()->AsScalar("frameIdx")->SetInt(frameIdx);

	if (checkTime > 5)
	{
		soundManager::Get()->stop("01loadingScene");
		soundManager::Get()->play("02loadingScene2",0.5f);
		SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::LoadingScene2);
		checkTime = 0;
	}
}

void LoadingScene::PreRender()
{

}

void LoadingScene::Render()
{
	render2d->Render();
}

void LoadingScene::PostRender()
{
	
}

void LoadingScene::ResizeScreen()
{
}
