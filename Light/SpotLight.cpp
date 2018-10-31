#include "stdafx.h"
#include "SpotLight.h"

SpotLight::SpotLight()
{
	buffer = new Buffer();

	isVisible = true;
}

SpotLight::~SpotLight()
{
	SAFE_DELETE(buffer);
}

void SpotLight::Update()
{
	buffer->SetPSBuffer(3);
	for (int i = 0; i < buffer->Data.Count; i++)
	{

	}
}

void SpotLight::Render()
{
	ImGui::Text("SpotLights");
	{
		if (ImGui::Button("IsVisibleSpot"))
		{
			IsDebugVisible();
		}
		for (int i = 0; i < buffer->Data.Count; i++)
		{
			ImGui::Text("Color[SpotLight%d]", i);
			ImGui::ColorEdit4((to_string(i) + "color").c_str(), buffer->Data.Lights[i].Color);
			ImGui::Separator();
			ImGui::Text("Direction[SpotLight%d]", i);
			ImGui::DragFloat3((to_string(i) + "Direction").c_str(), buffer->Data.Lights[i].Direction,0.1f,-1,1);
			ImGui::Separator();
			ImGui::Text("InnerAngle[SpotLight%d]", i);
			ImGui::DragFloat((to_string(i) + "InnerAngle").c_str(), &buffer->Data.Lights[i].InnerAngle, 0.1f,0.0f,1.0f);
			ImGui::Separator();
			ImGui::Text("OtterAngle[SpotLight%d]", i);
			ImGui::DragFloat((to_string(i) + "OtterAngle").c_str(), &buffer->Data.Lights[i].OuterAngle, 0.1f, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Pos[SpotLight%d]", i);
			ImGui::DragFloat3((to_string(i) + "pos").c_str(), buffer->Data.Lights[i].Position);
			ImGui::Separator();
		}
	}

	if (isVisible)
	{
		
	}
}

void SpotLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;
	
	buffer->Data.Count++;
}

void SpotLight::Remove()
{
	if (buffer->Data.Count == 0)
		return;
	buffer->Data.Count--;

}

void SpotLight::IsDebugVisible()
{
	if (isVisible)
		isVisible = false;
	else
		isVisible = true;
}
