#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"
#include "./Executes/ExeportModel.h"
#include "./Executes/TestAlphaBlend.h"
#include "./Executes/TestMultiTextures.h"
#include "./Executes/TestProjection.h"
#include "./Executes/TestShadow.h"
#include "./Executes/TestSky.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->GlobalLight = new LightBuffer();

	//TODO: 카메라메니저에서 설정함
	values->MainCamera = new Freedom(100);
	values->MainCamera->RotationDegree(10, 0);
	values->MainCamera->Position(30.13f, 177, -339.5f);

	values->GlobalLight->Data.Direction = { -1,-1,1 };
	executes.push_back(new TestSky(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values);
}

void Program::Update()
{
	values->MainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	D3DXMATRIX projection;
	values->Perspective->GetMatrix(&projection);

	D3DXMATRIX view;
	values->MainCamera->Matrix(&view);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	D3DXMATRIX projection;
	values->Perspective->GetMatrix(&projection);

	D3DXMATRIX view;
	values->MainCamera->Matrix(&view);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);
	values->Viewport->RSSetViewport();


	for (Execute* exe : executes)
		exe->Render();

	D3DDesc desc;
	D3D::GetDesc(&desc);
	RECT rc;
	GetClientRect(desc.Handle, &rc);

	ImGui::Text("%f %f", rc.right, rc.bottom);
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	RECT rect = { 0, 668, 300, 720 };
	DirectWrite::RenderText(str, rect, 14,L"돋움체",D3DXCOLOR(1,0,0,1),DWRITE_FONT_WEIGHT_BOLD);

	rect.top += 16;
	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	str = String::Format(L"CameraPos: %.0f, %.0f, %.0f", vec.x, vec.y, vec.x);
	DirectWrite::RenderText(str, rect, 14, L"돋움체", D3DXCOLOR(1, 0, 0, 1), DWRITE_FONT_WEIGHT_BOLD);

	rect.top += 16;
	D3DXVECTOR2 rot;
	values->MainCamera->Rotation(&rot);
	str = String::Format(L"CameraRot: %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 14, L"돋움체", D3DXCOLOR(1, 0, 0, 1), DWRITE_FONT_WEIGHT_BOLD);
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}