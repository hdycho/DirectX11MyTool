#include "stdafx.h"
#include "Mosaic.h"

Mosaic::Mosaic(ExecuteValues * values)
	:Render2D(values, Shaders + L"026_Mosaic.hlsl")
{
	buffer = new Buffer();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;
}

Mosaic::~Mosaic()
{
	SAFE_DELETE(buffer);
}

void Mosaic::Render()
{
	ImGui::Separator();
	//ImGui::SliderInt("Mosaic Select", &buffer->Data.Select, 0, 2);
	ImGui::SliderInt("Mosaic Size", &buffer->Data.Size, 1, 20);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
