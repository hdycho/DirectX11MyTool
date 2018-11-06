#pragma once

class Program;
class Window
{
public:
	Window();
	~Window();

	WPARAM Run();
	
	void ImGuiNewStyle();
	static void MouseNormalized(HWND&handle);	// 마우스가 화면밖으로나가면 중앙으로 위치시켜주는 함수 : FPS,TPS에서 사용

private:

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	static Program* program;
};