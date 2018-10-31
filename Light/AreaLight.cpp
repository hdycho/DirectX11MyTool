#include "stdafx.h"
#include "./Light/AreaLight.h"

AreaLight::AreaLight()
{
	buffer = new Buffer();
	isVisible = true;
}

AreaLight::~AreaLight()
{
	SAFE_DELETE(buffer);
}

void AreaLight::Update()
{
	buffer->SetPSBuffer(4);
	for (int i = 0; i < buffer->Data.Count; i++)
	{
		D3DXVECTOR2 value = { buffer->Data.Lights[i].Width ,buffer->Data.Lights[i].Height };
	}
}

void AreaLight::Render()
{
	ImGui::Text("AreaLights");
	{
		if (ImGui::Button("IsVisibleArea"))
		{
			IsDebugVisible();
		}
		for (int i = 0; i < buffer->Data.Count; i++)
		{
			ImGui::Text("Color[AreaLight%d]", i);
			ImGui::ColorEdit4((to_string(i) + "color").c_str(), buffer->Data.Lights[i].Color);
			ImGui::Separator();
			ImGui::Text("Intensity[AreaLight%d]", i);
			ImGui::DragFloat((to_string(i) + "intensity").c_str(), &buffer->Data.Lights[i].Intensity,0.1f, 0, 4);
			ImGui::Separator();
			ImGui::Text("Pos[AreaLight%d]", i);
			ImGui::DragFloat3((to_string(i) + "pos").c_str(), buffer->Data.Lights[i].Position);
			ImGui::Separator();
			ImGui::Text("Range[AreaLight%d]", i);
			ImGui::DragFloat((to_string(i) + "w").c_str(), &buffer->Data.Lights[i].Width);
			ImGui::SameLine();
			ImGui::DragFloat((to_string(i) + "h").c_str(), &buffer->Data.Lights[i].Height);
			ImGui::Separator();
		}
	}
	if (isVisible)
	{

	}
}

void AreaLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;
	buffer->Data.Count++;
}

void AreaLight::Remove()
{
	if (buffer->Data.Count == 0)
		return;
	buffer->Data.Count--;
}

void AreaLight::IsDebugVisible()
{
	if (isVisible)
		isVisible = false;
	else
		isVisible = true;
}
