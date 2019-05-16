#pragma once

class Window
{
public:
	static WPARAM Run(class IExecute* main);
	static void Progress(float val) { progress = val; }

	static void UseGameMouse();
	static void MouseNormalized(HWND&handle);	// 마우스가 화면밖으로나가면 중앙으로 위치시켜주는 함수 : FPS,TPS에서 사용
private:
	static void Create();
	static void Destroy();
	static void ProgressRender();
	static void MainRender();

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	static void ImGuiNewStyle();
private:
	static class IExecute* mainExecute;

	static bool bInitialize;
	static float progress;
	static mutex*mu;

	static bool isGameMouse;
};