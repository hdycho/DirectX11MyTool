#include "stdafx.h"
#include "Main.h"
#include "Systems\Window.h"
#include "Viewer\Freedom.h"
#include "Executes\Stage1.h"
#include "Executes\StartScene.h"
#include "Executes\LoadingScene.h"
#include "Executes\LoadingScene2.h"
#include "Executes\EndScene.h"

void Main::Initialize()
{
	SceneManager::Get()->InputScene(SceneManager::SCENETYPE::StartScene, new StartScene());
	SceneManager::Get()->InputScene(SceneManager::SCENETYPE::LoadingScene, new LoadingScene());
	SceneManager::Get()->InputScene(SceneManager::SCENETYPE::LoadingScene2, new LoadingScene2());
	SceneManager::Get()->InputScene(SceneManager::SCENETYPE::Stage1, new Stage1());
	SceneManager::Get()->InputScene(SceneManager::SCENETYPE::EndScene, new EndScene());
}

void Main::Ready()
{
	SceneManager::Get()->Init();
	SceneManager::Get()->Ready();
}

void Main::Destroy()
{
	ObjectManager::Delete();
}

void Main::Update()
{
	SceneManager::Get()->Update();
}

void Main::PreRender()
{
	SceneManager::Get()->PreRender();
}

void Main::Render()
{
	// Viewport ¹Ù²Ü²¨¶ó ¼¼ÆÃÇÏ´Â°Å
	Context::Get()->GetViewport()->RSSetViewport();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("BCKGND", NULL, ImGui::GetIO().DisplaySize, 0, 1 | 2 | 4 | 8 | 16 | 32 | 256 | 512 | 4096 | 8192);
	{
		float fps = ImGui::GetIO().Framerate;
		string str = "FPS : " + to_string(fps);
		ImGui::GetWindowDrawList()->AddText(ImVec2(10, 24), ImColor(1.0f, 0.0f, 0.0f, 1.0f), str.c_str());

		D3DXVECTOR3 vec;
		Context::Get()->GetMainCamera()->Position(&vec);
		str = "CameraPos : " + to_string((int)vec.x) + ", " + to_string((int)vec.y) + ", " + to_string((int)vec.z);
		ImGui::GetWindowDrawList()->AddText(ImVec2(10, 36), ImColor(1.0f, 0.0f, 0.0f, 1.0f), str.c_str());

		D3DXVECTOR2 rot;
		Context::Get()->GetMainCamera()->RotationDegree(&rot);
		str = "CameraRot : " + to_string((int)rot.x) + ", " + to_string((int)rot.y);
		ImGui::GetWindowDrawList()->AddText(ImVec2(10, 48), ImColor(1.0f, 0.0f, 0.0f, 1.0f), str.c_str());
	}
	ImGui::End();

	SceneManager::Get()->Render();
}

void Main::PostRender()
{
	SceneManager::Get()->PostRender();
}

void Main::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (Context::Get() != NULL) {
		Context::Get()->GetPerspective()->Set(desc.Width, desc.Height);
		Context::Get()->GetViewport()->Set(desc.Width, desc.Height);
	}

	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1600;
	desc.Height = 900;
	D3D::SetDesc(desc);


	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SAFE_DELETE(main);

	return wParam;
}