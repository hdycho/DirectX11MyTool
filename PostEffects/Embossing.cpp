#include "stdafx.h"
#include "Embossing.h"

Embossing::Embossing(ExecuteValues * values)
	:Render2D(values, Shaders + L"029_Embossing.hlsl")
{
	buffer = new Buffer();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;
}

Embossing::~Embossing()
{
}

void Embossing::Render()
{
	ImGui::Separator();
	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
