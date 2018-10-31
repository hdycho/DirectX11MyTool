#include "stdafx.h"
#include "Sharpening.h"

Sharpening::Sharpening(ExecuteValues * values)
	:Render2D(values, Shaders + L"030_Sharpening.hlsl")
{
	buffer = new Buffer();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;
}

Sharpening::~Sharpening()
{
}

void Sharpening::Render()
{
	ImGui::Separator();
	D3DXVECTOR2 scale = Scale();
	ImGui::SliderInt("Sharpness Intensity", &buffer->Data.Level, 1, 3);
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
