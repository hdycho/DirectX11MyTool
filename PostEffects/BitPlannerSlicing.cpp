#include "stdafx.h"
#include "BitPlannerSlicing.h"

BitPlannerSlicing::BitPlannerSlicing(ExecuteValues * values)
	:Render2D(values, Shaders + L"028_BitPlannerSlicing.hlsl")
{
	buffer = new Buffer();

	D3DXVECTOR2 scale = Scale();
}

BitPlannerSlicing::~BitPlannerSlicing()
{
	SAFE_DELETE(buffer);
}

void BitPlannerSlicing::Render()
{
	ImGui::Separator();
	//ImGui::SliderInt("BitPlannerSlicing Select", &buffer->Data.Select, 0, 2);
	ImGui::SliderInt("Bit Value", &buffer->Data.bitValue, 1, 7);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();

	buffer->SetPSBuffer(10);

	__super::Render();
}
