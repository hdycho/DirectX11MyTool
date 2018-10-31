#pragma once

class Program;
class Window
{
public:
	Window();
	~Window();

	WPARAM Run();
	
	void ImGuiNewStyle();
private:
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	static Program* program;
};