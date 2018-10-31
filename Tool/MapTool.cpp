#include "stdafx.h"
#include "MapTool.h"
#include "../LandScape/TerrainRender.h"
#include "../LandScape/Water.h"

MapTool::MapTool(ExecuteValues * values)
	:GameTool(L"MapTool"),values(values)
{

}

MapTool::~MapTool()
{
}

void MapTool::Update()
{
	switch (mSetType)
	{
		case MapTool::SET_NONE:
		break;
		case MapTool::SET_DETAIL_TERRAIN:
		{
			terrain->Update();
		}
		break;
		case MapTool::SET_BRUSH_TERRAIN:
		{
			D3DXVECTOR3 position;
			bool bPicked = terrain->Y(&position);
			if (bPicked)
			{
				if (terrain->brushType == 0)
				{
					if (Mouse::Get()->Press(0))
					{
						terrain->AdjustY(position);
					}
				}
				else if(terrain->brushType == 1)
				{
					if (Mouse::Get()->Press(0))
					{
						terrain->AdjustY(position);
					}
				}
			}
			terrain->Update();
		}
		break;
	}
}

void MapTool::Render()
{
	terrain->Render();
}

void MapTool::HirakyRender()
{
}

void MapTool::AssetRender()
{
}

void MapTool::InspectRender()
{
	BasicSetRender();

	switch (mSetType)
	{
		case SET_DETAIL_TERRAIN:
		{
			ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[TerrainFrustumInfo]");
			/*ImGui::Text("QuadTree Draw : %d", terrain->DrawCount());
			ImGui::Separator();*/
			ImGui::Text("FillMode");
			ImGui::SliderInt("fm", &terrain->RasterNum(), 0, 1);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[SetDetailInfo]");
			ImGui::Text("DetailIntensity");
			ImGui::DragFloat("dragIntensity", &terrain->PS_detailBuffer->Data.DetailPower, 0.01, 1, 10);
			ImGui::Text("DepthValue");
			ImGui::DragFloat2("depthValue", terrain->PS_detailBuffer->Data.Depth, 0.001, 0, 1);
			terrain->VS_detailBuffer->Data.VertexDepth = terrain->PS_detailBuffer->Data.Depth;

			ImGui::Text("TextureSize");
			ImGui::DragInt("textureSize", &terrain->VS_detailBuffer->Data.UvSize, 0.1, 1, 64);

			ImGui::Separator();
			ImGui::Separator();
		}
		break;
		case SET_BRUSH_TERRAIN:
		{
			ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[TerrainLine]");
			ImGui::Text("LineType");
			ImGui::SliderInt("lt", &terrain->lineBuffer->Data.Type, 0, 2);
			ImGui::Text("LineColor");
			ImGui::ColorEdit3("lineColor", terrain->lineBuffer->Data.Color);
			ImGui::Text("LineSpacing");
			ImGui::SliderInt("ls", &terrain->lineBuffer->Data.Spacing, 1, 10);
			ImGui::Text("LineThickness");
			ImGui::DragFloat("ltn", &terrain->lineBuffer->Data.Thickness, 0.1, 0.1,10);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[TerrainBrush]");
			ImGui::RadioButton("UpBrush", &terrain->brushType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("DownBrush", &terrain->brushType, 1);

			ImGui::Text("BrushType");
			ImGui::SliderInt("bt", &terrain->brushBuffer->Data.Type, 0, 3);
			ImGui::Text("BrushColor");
			ImGui::ColorEdit3("brushColor", terrain->brushBuffer->Data.Color);
			ImGui::Text("BrushRange");
			ImGui::SliderInt("br", &terrain->brushBuffer->Data.Range, 1, 100);
			ImGui::Text("BrushPower");
			ImGui::DragFloat("bp", &terrain->brushPower, 0.5f, 10,100);

			ImGui::Separator();
			ImGui::Separator();
		}
		break;
	}
}

void MapTool::InputTerrainPtr(TerrainRender * terrain)
{
	this->terrain = terrain;
}

TerrainRender * MapTool::GetTerrainPointer()
{
	return terrain;
}

void MapTool::LoadTerrainMapFile(TerrainRender * tree, wstring filename, int type)
{
	switch (type)
	{
		case 0:
		{
			tree->GetMaterial()->SetDiffuseMap(filename);
		}
		break;
		case 1:
		{
			tree->GetMaterial()->SetSpecularMap(filename);
		}
		break;
		case 2:
		{
			tree->GetMaterial()->SetNormalMap(filename);
		}
		break;
		case 3:
		{
			tree->GetMaterial()->SetDetailMap(filename);
		}
		break;
	}
}

void MapTool::BasicSetRender()
{
	ImGui::Checkbox("Visible", &(terrain->isVisible));
	ImGui::SameLine();
	ImGui::Checkbox("Active", &(terrain->isActive));
	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Setting Type]");
	ImGui::RadioButton("None", (int*)&mSetType, SET_NONE);
	ImGui::SameLine();
	ImGui::RadioButton("DetailSet", (int*)&mSetType, SET_DETAIL_TERRAIN);
	ImGui::SameLine();
	ImGui::RadioButton("BrushSet", (int*)&mSetType, SET_BRUSH_TERRAIN);
	ImGui::Separator();
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "=========[%s]=========", String::ToString(terrain->name).c_str());

	ImGui::Separator();
	ImGui::Separator();
	wstring textTrans = L"[" + terrain->name + L"]translate";
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
	ImGui::Text(String::ToString(textTrans).c_str());
	ImGui::DragFloat3((String::ToString(terrain->name) + "T").c_str(), (float*)&terrain->Position(), 0.05f, -128, 128);
	terrain->UpdateMatrix();

	wstring textScale = L"[" + terrain->name + L"]scale";
	ImGui::Text(String::ToString(textScale).c_str());
	ImGui::DragFloat3((String::ToString(terrain->name) + "S").c_str(), (float*)&terrain->Scale(), 0.05f, -10, 10);
	terrain->UpdateMatrix();

	wstring textRotate = L"[" + terrain->name + L"]rotate";
	ImGui::Text(String::ToString(textRotate).c_str());
	ImGui::DragFloat3((String::ToString(terrain->name) + "R").c_str(), (float*)&terrain->Rotate(), 0.05f, -3.14f, 3.14f);
	terrain->UpdateMatrix();

	ImGui::Separator();
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
	if (ImGui::TreeNode("Diffuse"))
	{
		ImGui::Text("Model DiffuseMap");
		if (terrain->GetMaterial()->GetDiffuseMap() != NULL)
		{
			if (ImGui::ImageButton(terrain->GetMaterial()->GetDiffuseMap()->GetView(), ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 0));
			}
		}
		else
		{
			if (ImGui::ImageButton(0, ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 0));
			}
		}
		ImGui::SameLine();
		ImGui::TreePop();
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
	if (ImGui::TreeNode("Specular"))
	{
		ImGui::Text("Model SpecularMap");

		if (terrain->GetMaterial()->GetSpecularMap() != NULL)
		{
			if (ImGui::ImageButton(terrain->GetMaterial()->GetSpecularMap()->GetView(), ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 1));
			}
		}
		else
		{
			if (ImGui::ImageButton(0, ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 1));
			}
		}
		ImGui::SameLine();
		ImGui::TreePop();
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
	if (ImGui::TreeNode("NormalMap"))
	{
		ImGui::Text("Model NormalMap");

		if (terrain->GetMaterial()->GetNormalMap() != NULL)
		{
			if (ImGui::ImageButton(terrain->GetMaterial()->GetNormalMap()->GetView(), ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 2));
			}
		}
		else
		{
			if (ImGui::ImageButton(0, ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 2));
			}
		}
		ImGui::SameLine();
		ImGui::TreePop();
	}
	ImGui::Separator();
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set DetailMap]");
	if (ImGui::TreeNode("DetailMap"))
	{
		ImGui::Text("Model DetailMap");

		if (terrain->GetMaterial()->GetNormalMap() != NULL)
		{
			if (ImGui::ImageButton(terrain->GetMaterial()->GetDetailMap()->GetView(), ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 3));
			}
		}
		else
		{
			if (ImGui::ImageButton(0, ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"颇老", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain, placeholders::_1, 3));
			}
		}
		ImGui::SameLine();
		ImGui::TreePop();
	}
	ImGui::Separator();
	ImGui::Separator();
}


