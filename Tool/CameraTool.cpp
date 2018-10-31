#include "stdafx.h"
#include "CameraTool.h"
#include "../Viewer/Frustum.h"

CameraTool::CameraTool(ExecuteValues * values)
	:GameTool(L"CameraTool"),values(values)
{
}

CameraTool::~CameraTool()
{
}

void CameraTool::Update()
{
}

void CameraTool::Render()
{
}

void CameraTool::HirakyRender()
{
}

void CameraTool::AssetRender()
{
}

void CameraTool::InspectRender()
{
	ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[FrustumInfo]");
	frustum->ImguiRender();
	
	ImGui::Separator();
	ImGui::Separator();
}
