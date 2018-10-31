#include "stdafx.h"
#include "PointLight.h"

PointLight::PointLight()
{
	buffer = new Buffer();
	isVisible = true;
}

PointLight::~PointLight()
{
	SAFE_DELETE(buffer);
}

void PointLight::Update()
{
	buffer->SetPSBuffer(2);
	for (int i = 0; i < buffer->Data.Count; i++)
	{
		
	}
}

void PointLight::Render()
{
	ImGui::Text("PointLights");
	{
		if (ImGui::Button("IsVisiblePoint"))
		{
			IsDebugVisible();
		}
		for (int i = 0; i < buffer->Data.Count; i++)
		{
			ImGui::Text("Color[PointLight%d]",i);
			ImGui::ColorEdit4((to_string(i)+"color").c_str(), buffer->Data.Lights[i].Color);
			ImGui::Separator();
			ImGui::Text("Intensity[PointLight%d]", i);
			ImGui::DragFloat((to_string(i) + "intensity").c_str(), &buffer->Data.Lights[i].Intensity,1,4);
			ImGui::Separator();
			ImGui::Text("Range[PointLight%d]", i);
			ImGui::DragFloat((to_string(i) + "range").c_str(), &buffer->Data.Lights[i].Range,1,50);
			ImGui::Separator();
			ImGui::Text("Pos[PointLight%d]", i);
			ImGui::DragFloat3((to_string(i) + "pos").c_str(), buffer->Data.Lights[i].Position);
			ImGui::Separator();
		}
	}

	if (isVisible)
	{

	}
}

void PointLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;
	buffer->Data.Count++;
}

void PointLight::Remove()
{
	if (buffer->Data.Count == 0)
		return;
	buffer->Data.Count--;
}

void PointLight::IsDebugVisible()
{
	if (isVisible)
		isVisible = false;
	else
		isVisible = true;
}

