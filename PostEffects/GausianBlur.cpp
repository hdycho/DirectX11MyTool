#include "stdafx.h"
#include "GausianBlur.h"

GausianBlur::GausianBlur(ExecuteValues * values, string psName)
	:Render2D(values, Shaders + L"030_GausianBlur.hlsl", "VS", psName)
{
	buffer = new Buffer();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;
}

GausianBlur::~GausianBlur()
{
	SAFE_DELETE(buffer);
}

void GausianBlur::Render()
{
	ImGui::Separator();
	ImGui::SliderInt("GausianBlur Count",&buffer->Data.Count, 0, 6);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);
	
	__super::Render();
}
