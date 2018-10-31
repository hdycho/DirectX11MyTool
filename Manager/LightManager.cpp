#include "stdafx.h"
#include "../Manager/LightManager.h"
#include "../Light/Shadow.h"

LightManager*LightManager::instance = NULL;


void LightManager::Init(ExecuteValues * values)
{
	sType = SHADOWTYPE::NONE;
	this->values = values;
	shadow = new Shadow(values);
}

LightManager * LightManager::GetInstance()
{
	if (instance == NULL)
	{
		instance = new LightManager();
	}
	return instance;
}

void LightManager::Update()
{
	switch (sType)
	{
		case LightManager::SHADOWTYPE::NONE:
		{

		}
		break;
		case LightManager::SHADOWTYPE::DEPTH_SHADOW:
		{
			shadow->Update();
		}
		break;
	}
}

void LightManager::Render()
{
	switch (sType)
	{
		case LightManager::SHADOWTYPE::NONE:
		{
		}
		break;
		case LightManager::SHADOWTYPE::DEPTH_SHADOW:
		{
			shadow->Render();
		}
		break;
	}
}

void LightManager::ImGuiRender()
{
	ImGui::Begin("LightManager",NULL, ImGuiWindowFlags_HorizontalScrollbar);
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Setting Type]");
		ImGui::RadioButton("NONE", (int*)&sType, (int)SHADOWTYPE::NONE);
		ImGui::SameLine();
		ImGui::RadioButton("DEPTH SHADOW", (int*)&sType, (int)SHADOWTYPE::DEPTH_SHADOW);
		
		switch (sType)
		{
			case LightManager::SHADOWTYPE::NONE:
			{
				ImGui::SliderFloat3("Direction", (float*)&values->GlobalLight->Data.Direction, -1, 1);
			}
			break;
			case LightManager::SHADOWTYPE::DEPTH_SHADOW:
			{
				shadow->ImGuiRender();
			}
			break;
		}
	}
	ImGui::End();
}

void LightManager::PreRender()
{
	switch (sType)
	{
		case LightManager::SHADOWTYPE::NONE:
		{

		}
		break;
		case LightManager::SHADOWTYPE::DEPTH_SHADOW:
		{
			shadow->PreRender();
		}
		break;
	}
}

bool LightManager::UseShadow()
{
	if (sType == SHADOWTYPE::NONE)
		return false;
	else
		return true;
}



