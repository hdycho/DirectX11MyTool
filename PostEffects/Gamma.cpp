#include "stdafx.h"
#include "Gamma.h"

Gamma::Gamma(ExecuteValues * values)
	:Render2D(values, Shaders + L"027_Gamma.hlsl")
{
	buffer = new Buffer();
}

Gamma::~Gamma()
{
	SAFE_DELETE(buffer);
}

void Gamma::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat3("Gamma Intensity", buffer->Data.Gamma, 1, 10);
	ImGui::Separator();
	buffer->SetPSBuffer(10);

	__super::Render();
}
