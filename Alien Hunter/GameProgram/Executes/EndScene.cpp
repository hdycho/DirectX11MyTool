#include "stdafx.h"
#include "EndScene.h"
#include "Viewer\Freedom.h"
#include "Systems\Window.h"

void EndScene::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(0, 0);
	Context::Get()->GetMainCamera()->Position(0, 0, 0);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);


	Window::UseGameMouse();
	mainCam = Context::Get()->GetMainCamera();
}

void EndScene::Ready()
{
	UIManager::Get()->SetUiRender(L"clearScene2", true);
	UIManager::Get()->UseFadeIn(-1.5f);
}

void EndScene::Destroy()
{
	Context::Get()->GetMainCamera() = mainCam;
}

void EndScene::Update()
{
	
	if (Keyboard::Get()->Down(VK_RETURN))
	{
		SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::StartScene);
		UIManager::Get()->SetUiRender(L"clearScene2", false);
		UIManager::Get()->ResetFadeOut();
	}
}

void EndScene::PreRender()
{

}

void EndScene::Render()
{
	
}

void EndScene::PostRender()
{

}

void EndScene::ResizeScreen()
{
}
