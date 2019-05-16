#include "stdafx.h"
#include "StartScene.h"
#include "Viewer\Freedom.h"
#include "Environment\Sky.h"
#include "Environment\Terrain.h"
#include "Environment\Billboard.h"
#include "Environment\Rain.h"
#include "Environment\Rain.h"
#include "Environment\Shadow.h"
#include "Systems\Window.h"

void StartScene::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(0, 0);
	Context::Get()->GetMainCamera()->Position(0, 0, 0);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);


	Window::UseGameMouse();
	mainCam = Context::Get()->GetMainCamera();
	dcImage = new Render2D(L"023_mosaic.hlsl");

	startImage = new Texture(Textures + L"GameScene/StartImage.png");

	float width=Context::Get()->GetViewport()->GetWidth();
	float height= Context::Get()->GetViewport()->GetHeight();

	dcImage->SRV(startImage->SRV());
	dcImage->Position(0, 0);
	dcImage->Scale(width, height);

	size = 1;
	alpha = 1;
	isStart = false;
	init = false;

	dcImage->GetEffect()->AsScalar("width")->SetFloat(width);
	dcImage->GetEffect()->AsScalar("height")->SetFloat(height);

	dcImage->GetEffect()->AsScalar("size")->SetFloat(size);
}

void StartScene::Ready()
{

}

void StartScene::Destroy()
{
	Context::Get()->GetMainCamera() = mainCam;

	SAFE_DELETE(dcImage);
}

void StartScene::Update()
{
	if (!init)
	{
		soundManager::Get()->play("00startScene",0.2f);
		init = true;
	}
	ShowCursor(false);
	ShowCursor(false);

	dcImage->Update();
	
	static float time = 0;
	
	dcImage->GetEffect()->AsScalar("size")->SetFloat(size);
	dcImage->GetEffect()->AsScalar("alphaValue")->SetFloat(alpha);

	if (Keyboard::Get()->Down(VK_RETURN))
	{
		soundManager::Get()->play("05Select", 0.8f);
		isStart = true;
	}

	if (isStart)
	{
		time += Time::Get()->Delta();
		
		if (time > 0.1f)
		{
			if(size<=30)
				size++;
			time = 0;
		}

		if (size > 30)
		{
			static float alphaTime = 0;
			
			alphaTime += Time::Get()->Delta();

			if (alphaTime > 0.05f)
			{
				alpha -= 0.05f;
				alphaTime = 0;
			}
			
			if (alpha < 0)
			{
				size = 1;
				alphaTime = 0;
				SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::LoadingScene);
				soundManager::Get()->stop("00startScene");
				soundManager::Get()->play("01loadingScene");
			}
		}
	}
}

void StartScene::PreRender()
{

}

void StartScene::Render()
{

}

void StartScene::PostRender()
{
	dcImage->Render();
}

void StartScene::ResizeScreen()
{
}
