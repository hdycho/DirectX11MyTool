#include "stdafx.h"
#include "Main.h"
#include "Systems\Window.h"
#include "Viewer\Freedom.h"
#include "Executes\TestTool.h"


void Main::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(0, 0);
	Context::Get()->GetMainCamera()->Position(0, 0, 0);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);

	Push(new TestTool());
}

void Main::Ready()
{
	for (IExecute* exe : executes)
		exe->Ready();

}

void Main::Destroy()
{
	for (IExecute* exe : executes) {
		exe->Destroy();
		SAFE_DELETE(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	// Viewport ¹Ù²Ü²¨¶ó ¼¼ÆÃÇÏ´Â°Å
	Context::Get()->GetViewport()->RSSetViewport();

	//if (ImGui::DragFloat3("Direction", (float*)&Context::Get()->GetGlobalLight()->Direction, 0.1f, -1.0f, 1.0f))
	//	Context::Get()->ChangeGlobalLight();

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
		Context::Get()->GetMainCamera()->Rotation(&rot);
		str = "CameraRot : " + to_string((int)rot.x) + ", " + to_string((int)rot.y);
		ImGui::GetWindowDrawList()->AddText(ImVec2(10, 48), ImColor(1.0f, 0.0f, 0.0f, 1.0f), str.c_str());
	}
	ImGui::End();

	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();

	/*wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	RECT rect = { 0, 668, 300, 720 };
	DirectWrite::RenderText(str, rect, 14, L"µ¸¿òÃ¼", D3DXCOLOR(1, 0, 0, 1), DWRITE_FONT_WEIGHT_BOLD);

	rect.top += 16;
	D3DXVECTOR3 vec;
	Context::Get()->GetMainCamera()->Position(&vec);
	str = String::Format(L"CameraPos: %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
	DirectWrite::RenderText(str, rect, 14, L"µ¸¿òÃ¼", D3DXCOLOR(1, 0, 0, 1), DWRITE_FONT_WEIGHT_BOLD);

	rect.top += 16;
	D3DXVECTOR2 rot;
	Context::Get()->GetMainCamera()->Rotation(&rot);
	str = String::Format(L"CameraRot: %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 14, L"µ¸¿òÃ¼", D3DXCOLOR(1, 0, 0, 1), DWRITE_FONT_WEIGHT_BOLD);*/
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