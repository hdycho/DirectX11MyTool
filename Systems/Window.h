#pragma once

class Program;
class Window
{
public:
	Window();
	~Window();

	WPARAM Run();
	
	void ImGuiNewStyle();
	static void MouseNormalized(HWND&handle);	// ���콺�� ȭ������γ����� �߾����� ��ġ�����ִ� �Լ� : FPS,TPS���� ���

private:

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	static Program* program;
};