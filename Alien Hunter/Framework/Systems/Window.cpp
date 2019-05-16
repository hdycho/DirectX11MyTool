#include "Framework.h"
#include "Window.h"
#include "IExecute.h"

IExecute* Window::mainExecute = NULL;

bool Window::bInitialize = false;
float Window::progress = 0.0f;
mutex* Window::mu = NULL;
bool Window::isGameMouse = false;

WPARAM Window::Run(IExecute * main)
{
	//_CrtSetBreakAlloc(1128);
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
#endif
	srand(static_cast<UINT>(time(nullptr)));

	mainExecute = main;

	Create(); // 윈도우 생성

	D3DDesc desc;
	D3D::GetDesc(&desc);

	//===================싱글톤 초기화====================//
	D3D::Create();
	Keyboard::Create();
	Mouse::Create();

	Time::Create();
	Time::Get()->Start();

	SceneManager::Create();
	ColliderManager::Create();
	UIManager::Create();
	InstanceManager::Create();
	ParticleManager::Create();
	Context::Create();
	FileManager::Create();
	soundManager::Create();
	soundManager::Get()->init();
	//==================================================//


	ImGui::Create(desc.Handle, D3D::GetDevice(), D3D::GetDC());
	ImGui::StyleColorsDark();
	ImGuiNewStyle();

	mu = new mutex();
	thread t = thread([&]()
	{
		mainExecute->Initialize();
		
		mu->lock();
		bInitialize = true;
		mu->unlock();
	});

	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (mu != NULL)
			{
				bool temp = false;
				mu->lock();
				temp = bInitialize;
				mu->unlock();

				if (temp == false)
					ProgressRender();
				else
				{
					t.join();
					SAFE_DELETE(mu);

					mainExecute->Ready();
					UIManager::Get()->Ready();
				}
			}
			else
			{
				MainRender();
			}
		}
	}
	mainExecute->Destroy();

	//===================싱글톤 해제=====================//
	InstanceManager::Delete();
	Context::Delete();
	Textures::Delete();
	Effects::Delete();
	FileManager::Delete();
	BulletManager::Delete();
	ParticleManager::Delete();
	UIManager::Delete();
	soundManager::Delete();

	ColliderManager::Delete();
	SceneManager::Delete();
	Models::Delete();

	ImGui::Delete();
	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();
	D3D::Delete();
	//==================================================//

	Destroy();

	return msg.wParam;
}

void Window::UseGameMouse()
{
	isGameMouse = true;
}

void Window::MouseNormalized(HWND & handle)
{
	if (!isGameMouse)return;
	//마우스가 클라이언트화면 밖으로 넘어가면 중앙으로 위치시키기

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(handle, &point);

	//먼저 클라이언트 영역의 크기부터 받아온다
	RECT rc;
	GetClientRect(handle, &rc);

	//x축
	if (point.x > rc.right|| point.x<rc.left)
	{
		POINT sPoint = { rc.left,rc.right };

		ClientToScreen(handle, &sPoint);
		ClientToScreen(handle, &point);
		int midPosX = (sPoint.x + sPoint.y) / 2;
		SetCursorPos(midPosX, point.y);
	}

	//y축
	if (point.y > rc.bottom || point.y < rc.top)
	{
		POINT sPoint = { rc.bottom,rc.top };

		ClientToScreen(handle, &sPoint);
		ClientToScreen(handle, &point);
		int midPosY = (sPoint.x + sPoint.y) / 2;
		SetCursorPos(point.x, midPosY);
	}
}

void Window::Create()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);


	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);
	D3D::SetDesc(desc);


	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);

	ShowCursor(true);
}

void Window::Destroy()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (desc.bFullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

void Window::ProgressRender()
{
	Time::Get()->Update();

	if (ImGui::IsMouseHoveringAnyWindow() == false)
	{
		Keyboard::Get()->Update();
		Mouse::Get()->Update();
	}

	ImGui::Update();

	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(D3DXCOLOR(0, 0, 0, 1));
	{
		// 여기다 로딩 화면 띄울껄 렌더 하면 됨
		// Initialize 도중은 여기서 렌더할 꺼
		progress += Time::Get()->Delta()*0.9f;
		ImGui::ProgressBar(progress);
		ImGui::Render();
	}
	D3D::Get()->Present();
}

void Window::MainRender()
{
	Time::Get()->Update();
	//SetCursor(false);
	if (ImGui::IsMouseHoveringAnyWindow() == false)
	{
		Keyboard::Get()->Update();
		Mouse::Get()->Update();
	}

	//===============게임 업데이트==============//
	Context::Get()->Update();
	mainExecute->Update();
	InstanceManager::Get()->Update();
	UIManager::Get()->Update();
	BulletManager::Get()->BulletUpdate();
	ColliderManager::Get()->Update();
	ParticleManager::Get()->Update();
	soundManager::Get()->Update();
	//=========================================//


	ImGui::Update();

	mainExecute->PreRender();

	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(D3DXCOLOR(0, 0, 0, 1));
	{
		mainExecute->Render();
		BulletManager::Get()->BulletRender();

		ImGui::Render();

		{
			ParticleManager::Get()->Render();
			mainExecute->PostRender();
			UIManager::Get()->Render();
		}
	}
	D3D::Get()->Present();
	//SetCursor(false);
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Get()->InputProc(message, wParam, lParam);

	if (ImGui::WndProc((UINT*)handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		ImGui::Invalidate();
		{
			if (mainExecute != NULL)
			{
				float width = (float)LOWORD(lParam);
				float height = (float)HIWORD(lParam);


				if (D3D::Get() != NULL)
					D3D::Get()->ResizeScreen(width, height);


				mainExecute->ResizeScreen();
			}
		}
		ImGui::Validate();
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void Window::ImGuiNewStyle()
{
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.80f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	ImGuiIO io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 12);
	//io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 10);
	//io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 14);
	//io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 18);
}
