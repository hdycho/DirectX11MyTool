#include "stdafx.h"
#include "MapTool.h"
#include "Environment/Terrain.h"
#include "Environment/TerrainRender.h"
#include "Environment/HeightMap.h"
#include "Environment/CubeSky.h"
#include "Environment/Sky.h"
#include "Environment/Rain.h"
#include "Environment/Shadow.h"
#include "Environment\Billboard.h"
#include "Collider\ColliderBox.h"

#include "Utilities/Xml.h"
#include "Systems/Window.h"
#include "Draw/GizmoGrid.h"

MapTool::MapTool()
{
	grid = new GizmoGrid();
	selectObjName = L"";

	shadow = new Shadow(2048, 2048);
	shadow->Initialize();

	brushEffect = new Effect(Effects + L"007_TerrainBrush.hlsl", true);
	lineEffect = new Effect(Effects + L"001_Line.hlsl");

	selectType = L" None";
	typeNames.push_back(L" None");
	typeNames.push_back(L" Detail_Terrain");
	typeNames.push_back(L" Height_Terrain");
	typeNames.push_back(L" Brush_Terrain");
	typeNames.push_back(L" Blend_Terrain");
	typeNames.push_back(L" Fog_Terrain");
	typeNames.push_back(L" Billboard_Terrain");

	tagNames.push_back(L" Terrains");
	tagNames.push_back(L" Skys");
	tagNames.push_back(L" Rains");

	for (int i = 0; i < 3; i++)
		layerImg[i] = NULL;
}

MapTool::~MapTool()
{
	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end();tIter++)
	{
		SAFE_DELETE(tIter->second);
	}
	terrains.clear();

	cubeSkyIter cIter = cubeSkys.begin();
	for (; cIter != cubeSkys.end();cIter++)
	{
		SAFE_DELETE(cIter->second);
	}
	cubeSkys.clear();

	SkyIter sIter = skys.begin();
	for (; sIter != skys.end();sIter++)
	{
		SAFE_DELETE(sIter->second);
	}
	skys.clear();

	rainIter rIter = rains.begin();
	for (; rIter != rains.end();rIter++)
	{
		SAFE_DELETE(rIter->second);
	}
	rains.clear();

	SAFE_DELETE(shadow);
	SAFE_DELETE(grid);

	SAFE_DELETE(brushEffect);
	SAFE_DELETE(lineEffect);
	SAFE_RELEASE(srv);
	SAFE_RELEASE(uav);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(layerImg[i]);

	//for (Terrain*terrain : loadTerrains)
	//	SAFE_DELETE(terrain);

	for (Sky*sky : loadSkys)
		SAFE_DELETE(sky);

	for (Rain*rain : loadRains)
		SAFE_DELETE(rain);
}

void MapTool::Ready()
{
	CreateComputeTexture();
	shadow->Ready();
}

void MapTool::Render()
{
	//================================기본창================================//
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Data"))
				{
					mType = LOAD_DATA;
				}
				if (ImGui::MenuItem("Save Data"))
				{
					mType = SAVE_DATA;
				}
				if (ImGui::MenuItem("Exit"))
				{
					PostQuitMessage(0);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("GameScene"))
				{
					sType = GameTool::SCENETYPE::GAMESCENE;
				}
				if (ImGui::MenuItem("EditScene"))
				{
					sType = GameTool::SCENETYPE::EDITSCENE;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create Objects"))
			{
				if (ImGui::BeginMenu("Environment"))
				{
					if (ImGui::MenuItem("Terrain"))
					{
						static int terrainNum = 0;

						//=====================Create Terrain=====================//
						Terrain::InitDesc desc;
						desc.HeightMapInit(Textures + L"HeightMap/terrain(normal).raw", 60.0f);

						vector<wstring> layerMaps;
						layerMaps.push_back(Textures + L"darkdirt.png");
						layerMaps.push_back(Textures + L"stone.png");
						layerMaps.push_back(Textures + L"snow.png");

						desc.BlendMapInit(layerMaps);

						//=========================================================//

						terrains[L"Terrain" + to_wstring(terrainNum)] = new Terrain(desc);
						terrains[L"Terrain" + to_wstring(terrainNum)]->Initialize();
						terrains[L"Terrain" + to_wstring(terrainNum)]->Name() = L"Terrain" + to_wstring(terrainNum);
						terrains[L"Terrain" + to_wstring(terrainNum)]->Ready();
						FileManager::Get()->DataPush(FileManager::FileType::Terrain, terrains[L"Terrain" + to_wstring(terrainNum)]);


						for (int i = 0; i < 3; i++)
						{
							if (layerImg[i] == NULL)
								layerImg[i] = new Texture(terrains[L"Terrain" + to_wstring(terrainNum)]->Desc().layerMapFile[i]);
						}

						shadow->Add(terrains[L"Terrain" + to_wstring(terrainNum)]);


						ObjectNames.push_back(L"Terrain" + to_wstring(terrainNum));
						terrainNum++;
					}
					if (ImGui::MenuItem("CubeSky"))
					{
						static int cubeSkyNum = 0;
						cubeSkys[L"CubeSky" + to_wstring(cubeSkyNum)] = new CubeSky(Textures + L"sunsetcube1024.dds");
						cubeSkys[L"CubeSky" + to_wstring(cubeSkyNum)]->Name() = L"CubeSky" + to_wstring(cubeSkyNum);

						ObjectNames.push_back(L"CubeSky" + to_wstring(cubeSkyNum));
						cubeSkyNum++;
					}
					if (ImGui::MenuItem("ScatterSky"))
					{
						static int skyNum = 0;
						skys[L"Sky" + to_wstring(skyNum)] = new Sky();
						skys[L"Sky" + to_wstring(skyNum)]->Initialize();
						skys[L"Sky" + to_wstring(skyNum)]->Ready();
						skys[L"Sky" + to_wstring(skyNum)]->Name() = L"Sky" + to_wstring(skyNum);
						FileManager::Get()->DataPush(FileManager::FileType::Sky, skys[L"Sky" + to_wstring(skyNum)]);

						ObjectNames.push_back(L"Sky" + to_wstring(skyNum));
						skyNum++;
					}
					if (ImGui::MenuItem("Rain"))
					{
						static int rainNum = 0;
						rains[L"Rain" + to_wstring(rainNum)] = new Rain(D3DXVECTOR3(100, 100, 100), 1000);
						rains[L"Rain" + to_wstring(rainNum)]->Ready();
						rains[L"Rain" + to_wstring(rainNum)]->Name() = L"Rain" + to_wstring(rainNum);
						FileManager::Get()->DataPush(FileManager::FileType::Rain, rains[L"Rain" + to_wstring(rainNum)]);

						ObjectNames.push_back(L"Rain" + to_wstring(rainNum));
						rainNum++;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	//=====================================================================//

	if (sType == SCENETYPE::EDITSCENE)
	{
		//인스펙터창
		DrawInspector();
		//에셋창
		DrawAsset();
		//하이라키 창
		DrawHiarachy();

		grid->Render();
	}


	//==============================환경 랜더=================================//
	//스카이
	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->Render();

	shadow->Render();

	//큐브스카이
	cubeSkyIter cIter = cubeSkys.begin();
	for (; cIter != cubeSkys.end(); cIter++)
		cIter->second->Render();

	//레인
	rainIter rIter = rains.begin();
	for (; rIter != rains.end(); rIter++)
		rIter->second->Render();
	//======================================================================//
}


void MapTool::Update()
{
	//=================================FBX추출=================================//
	switch (mType)
	{
		case MapTool::LOAD_DATA:
		{
			Path::OpenFileDialog(L"파일", L".png", Datas, bind(&MapTool::LoadMapData, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case MapTool::SAVE_DATA:
		{
			//여기선 파일하나하나 저장할수있게해야함

			char*pathName = savePath;
			string path = pathName;

			if (path.size()<1)
				FileManager::Get()->Save(L"");
			else
				FileManager::Get()->SaveWithFolder(String::ToWString(path));

			mType = MENU_NONE;
		}
		break;
	}
	//======================================================================//


	//터레인
	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		tIter->second->Update();

	//큐브스카이
	cubeSkyIter cIter = cubeSkys.begin();
	for (; cIter != cubeSkys.end(); cIter++)
		cIter->second->Update();

	//스카이
	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->Update();

	//레인
	rainIter rIter = rains.begin();
	for (; rIter != rains.end(); rIter++)
		rIter->second->Update();

	shadow->Update();
}

void MapTool::PreRender()
{

	//스카이
	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->PreRender();

	//큐브스카이
	cubeSkyIter cIter = cubeSkys.begin();
	for (; cIter != cubeSkys.end(); cIter++)
		cIter->second->PreRender();

	shadow->PreRender();
}

void MapTool::PostRender()
{

}


void MapTool::PickedInit()
{
	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		tIter->second->SetPickState(false);

	cubeSkyIter cIter = cubeSkys.begin();
	if (cIter != cubeSkys.end())
		cIter->second->SetPickState(false);


	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->SetPickState(false);

	rainIter rIter = rains.begin();
	for (; rIter != rains.end(); rIter++)
		rIter->second->SetPickState(false);
}

void MapTool::DrawAsset()
{
	ImGui::Begin("Asset", NULL);
	{
		ImGui::Separator();
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Set SaveFolderName]");
		ImGui::Text("SaveFolder");
		ImGui::InputText("saveFolder", savePath, sizeof(savePath));
		ImGui::Separator();
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Class Tag]");
		if (ImGui::BeginCombo("Tag", String::ToString(selectTagName).c_str()))
		{
			for (UINT i = 0; i < tagNames.size(); i++)
			{
				bool isSelected = (selectTagName == tagNames[i]);
				if (ImGui::Selectable(String::ToString(tagNames[i]).c_str(), isSelected))
				{
					selectTagName = tagNames[i];
					int a = 10;
				}
				if (isSelected == true)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "============[AssetFile]============");

		if (selectTagName == L" Terrains")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Terrain Datas]: %d", loadTerrains.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("terrain", ImGuiDir_Down))
			{
				if (!aType.showTerrain)aType.showTerrain = true;
				else aType.showTerrain = false;
			}
			if (selectLoadDataName == L"")
				loadDataColor = { 1,0,0 };
			else
				loadDataColor = { 0,0,1 };
			ImGui::ColorButton("CurrentData", ImVec4(loadDataColor.x, loadDataColor.y, loadDataColor.z, 1));
			ImGui::SameLine();

			if (ImGui::Button("PushData"))
			{
				if (loadDataColor.z == 1)
				{
					for (UINT i = 0; i < loadTerrains.size(); i++)
					{
						if (loadTerrains[i]->Name() == selectLoadDataName)
						{
							Terrain*copy = new Terrain(*loadTerrains[i]);
							copy->Initialize();
							copy->GetBillboard()->SetLineEffect(lineEffect);
							for (UINT i = 0; i < copy->GetBillboard()->GetBillboardCollider().size(); i++)
							{
								copy->GetBillboard()->GetBillboardCollider()[i].Box->CreateDebugLine(lineEffect);
							}
							copy->Ready();

							terrains[loadTerrains[i]->Name()] = copy;
							FileManager::Get()->DataPush(FileManager::FileType::Terrain, terrains[loadTerrains[i]->Name()]);
							shadow->Add(terrains[loadTerrains[i]->Name()]);
							shadow->Add(terrains[loadTerrains[i]->Name()]->GetBillboard());
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<Terrain*>::iterator viter;
				viter = loadTerrains.begin();
				for (; viter != loadTerrains.end(); viter++)
				{
					if ((*viter)->Name() == selectLoadDataName)
					{
						//SAFE_DELETE(*viter); //TODO:오류구간
						loadTerrains.erase(viter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showTerrain)
			{
				for (UINT i = 0; i < loadTerrains.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadTerrains[i]->Name())).c_str()))
					{
						selectLoadDataName = loadTerrains[i]->Name();
						if (ImGui::TreeNode("Material"))
						{
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Texture Maps");
							ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)",
								loadTerrains[i]->Desc().material->GetDiffuse().r,
								loadTerrains[i]->Desc().material->GetDiffuse().g,
								loadTerrains[i]->Desc().material->GetDiffuse().b);

							ImGui::Text("Diffuse Map");
							if (loadTerrains[i]->Desc().material->GetDiffuseMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadTerrains[i]->Desc().material->GetDiffuseMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();

							ImGui::Text("Specular Color : (%.2f,%.2f,%.2f,%.2f)",
								loadTerrains[i]->Desc().material->GetSpecular().r,
								loadTerrains[i]->Desc().material->GetSpecular().g,
								loadTerrains[i]->Desc().material->GetSpecular().b,
								loadTerrains[i]->Desc().material->GetSpecular().a);
							ImGui::Text("Specular Map");
							if (loadTerrains[i]->Desc().material->GetSpecularMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadTerrains[i]->Desc().material->GetSpecularMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::Text("Normal Map");
							if (loadTerrains[i]->Desc().material->GetNormalMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadTerrains[i]->Desc().material->GetNormalMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::TreePop();
						}
						if (ImGui::TreeNode("HeightMap"))
						{
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "HeightMapInfo");
							ImGui::Text("HeightMap Texture");
							ImGui::Image(loadTerrains[i]->GetTerrainRender()->GetHeightMapSRV(), ImVec2(80, 80));
							ImGui::Separator();
							ImGui::BulletText("HeightScale");
							ImGui::SameLine();
							ImGui::Text("%.f", loadTerrains[i]->Desc().HeightScale);
							ImGui::Separator();
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("terrainLoad", &loadTerrains[i], sizeof(Terrain*));
						ImGui::Text(String::ToString(loadTerrains[i]->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}
		else if (selectTagName == L" Skys")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Sky Datas]: %d", loadSkys.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("sky", ImGuiDir_Down))
			{
				if (!aType.showSky)aType.showSky = true;
				else aType.showSky = false;
			}
			if (selectLoadDataName == L"")
				loadDataColor = { 1,0,0 };
			else
				loadDataColor = { 0,0,1 };
			ImGui::ColorButton("CurrentData", ImVec4(loadDataColor.x, loadDataColor.y, loadDataColor.z, 1));
			ImGui::SameLine();

			if (ImGui::Button("PushData"))
			{
				if (loadDataColor.z == 1)
				{
					for (UINT i = 0; i < loadSkys.size(); i++)
					{
						if (loadSkys[i]->Name() == selectLoadDataName)
						{
							Sky*copy = new Sky(*loadSkys[i]);
							copy->Initialize();
							copy->Ready();
							copy->Name() = loadSkys[i]->Name();

							skys[loadSkys[i]->Name()] = copy;
							FileManager::Get()->DataPush(FileManager::FileType::Sky, skys[loadSkys[i]->Name()]);
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<class Sky*>::iterator viter;
				viter = loadSkys.begin();
				for (; viter != loadSkys.end(); viter++)
				{
					if ((*viter)->Name() == selectLoadDataName)
					{
						SAFE_DELETE(*viter);
						loadSkys.erase(viter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showSky)
			{
				for (UINT i = 0; i < loadSkys.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadSkys[i]->Name())).c_str()))
					{
						selectLoadDataName = loadSkys[i]->Name();
						if (ImGui::TreeNode("Texture Maps"))
						{
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Textures");
							ImGui::Text("StarMap");
							if (loadSkys[i]->GetStarFieldMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadSkys[i]->GetStarFieldMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();

							ImGui::Text("MoonMap");
							if (loadSkys[i]->GetMoonMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadSkys[i]->GetMoonMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::Text("GlowMap");
							if (loadSkys[i]->GetGlowMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadSkys[i]->GetGlowMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::Text("CloudMap");
							if (loadSkys[i]->GetCloudMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadSkys[i]->GetCloudMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::TreePop();
						}
						ImGui::Text("StartTheta : %.2f", loadSkys[i]->StartTheta());
						ImGui::Text("TimeFactor : %.2f", loadSkys[i]->TimeFactor());

						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("skyLoad", &loadSkys[i], sizeof(Sky*));
						ImGui::Text(String::ToString(loadSkys[i]->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}
		else if (selectTagName == L" Rains")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Rain Datas]: %d", loadRains.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("rain", ImGuiDir_Down))
			{
				if (!aType.showRain)aType.showRain = true;
				else aType.showRain = false;
			}

			if (selectLoadDataName == L"")
				loadDataColor = { 1,0,0 };
			else
				loadDataColor = { 0,0,1 };
			ImGui::ColorButton("CurrentData", ImVec4(loadDataColor.x, loadDataColor.y, loadDataColor.z, 1));
			ImGui::SameLine();

			if (ImGui::Button("PushData"))
			{
				if (loadDataColor.z == 1)
				{
					for (UINT i = 0; i < loadRains.size(); i++)
					{
						if (loadRains[i]->Name() == selectLoadDataName)
						{
							Rain*copy = new Rain(*loadRains[i]);

							copy->Ready();
							copy->Name() = loadRains[i]->Name();

							rains[loadRains[i]->Name()] = copy;
							FileManager::Get()->DataPush(FileManager::FileType::Rain, rains[loadRains[i]->Name()]);
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<class Rain*>::iterator viter;
				viter = loadRains.begin();
				for (; viter != loadRains.end(); viter++)
				{
					if ((*viter)->Name() == selectLoadDataName)
					{
						SAFE_DELETE(*viter);
						loadRains.erase(viter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showRain)
			{
				for (UINT i = 0; i < loadRains.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadRains[i]->Name())).c_str()))
					{
						selectLoadDataName = loadRains[i]->Name();
						ImGui::Text("Size : [%.2f,%.2f,%.2f]", loadRains[i]->size.x, loadRains[i]->size.y, loadRains[i]->size.z);
						ImGui::Text("RainCount : %d", loadRains[i]->particleCount);
						ImGui::Text("DrawDistance : %.2f", loadRains[i]->drawDist);
						ImGui::Text("Velocity : %.2f", loadRains[i]->velocity.x, loadRains[i]->velocity.y, loadRains[i]->velocity.z);
						ImGui::Separator();
						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("rainLoad", &loadRains[i], sizeof(Rain*));
						ImGui::Text(String::ToString(loadRains[i]->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}

	}
	ImGui::End();
}

void MapTool::DrawInspector()
{
	ImGui::Begin("Inspector", NULL);
	{
		//======================================터레인=====================================//
		terrainIter tIter = terrains.begin();
		for (; tIter != terrains.end(); tIter++)
		{
			if (tIter->second->IsPicked())
			{
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "==============[%s]==============", String::ToString(tIter->first).c_str());
				ImGui::TextColored(ImVec4(0.8f, 0, 0.8f, 1), "[Setting Type]");
				if (ImGui::BeginCombo("MapToolType", String::ToString(selectType).c_str()))
				{
					for (size_t i = 0; i < typeNames.size(); i++)
					{
						bool isSelected = (selectType == typeNames[i]);
						if (ImGui::Selectable(String::ToString(typeNames[i]).c_str(), isSelected))
						{
							selectType = typeNames[i];
							CheckSelectType();
						}
						if (isSelected == true)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				TerrainTypeRender(tIter->second);
				ImGui::Separator();
				ImGui::Separator();
			}
		}
		//==================================================================================//

		//======================================큐브스카이=====================================//
		cubeSkyIter cIter = cubeSkys.begin();
		for (; cIter != cubeSkys.end(); cIter++)
		{
			if (cIter->second->IsPicked())
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "==============[%s]==============", String::ToString(cIter->first).c_str());
				ImGui::Separator();
				ImGui::Separator();
				if (ImGui::TreeNode("CubeMap"))
				{
					if (cIter->second->SRV() != NULL)
					{
						ImGui::BulletText("MapFile");
						ImGui::TextColored(ImVec4(1, 1, 0, 1), String::ToString(Path::GetFileName(cIter->second->GetFileName())).c_str());
						if (ImGui::ImageButton(cIter->second->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadCubeSkyMapFile, this, cIter->second, placeholders::_1));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadCubeSkyMapFile, this, cIter->second, placeholders::_1));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::Separator();
			}
		}


		//==================================================================================//

		//======================================스카이=====================================//
		SkyIter sIter = skys.begin();
		for (; sIter != skys.end(); sIter++)
		{
			if (sIter->second->IsPicked())
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "================[%s]================", String::ToString(sIter->first).c_str());
				ImGui::Separator();
				ImGui::Separator();
				ImGui::Text("LightDirection");
				ImGui::BulletText("[X:%.2f] [Y:%.2f] [Z:%.2f]", Context::Get()->GetGlobalLight()->Direction.x, Context::Get()->GetGlobalLight()->Direction.y, Context::Get()->GetGlobalLight()->Direction.z);

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Sun]");
				ImGui::Text("Ambient");
				ImGui::ColorEdit3("##amcolor", Context::Get()->GetGlobalLight()->Ambient);
				ImGui::Text("SunDirection");
				ImGui::SliderFloat("##Theta", &sIter->second->Theta(), 0.0f, Math::PI * 2.0f, "%.5f");
				ImGui::Text("StartAngle");
				ImGui::SliderFloat("##Start", &sIter->second->StartTheta(), 0.0f, Math::PI * 2.0f, "%.5f");
				ImGui::Text("SunLeans");
				ImGui::DragFloat("##Leans", &sIter->second->Leans(), 1, 0, 90);
				ImGui::Text("TimeFactor");
				ImGui::DragFloat("##Time Factor", &sIter->second->TimeFactor(), 0.001f, -1.0f, 1.0f);
				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set StarMap]");
				if (ImGui::TreeNode("StarMap"))
				{
					ImGui::Text("Sky StarMap");

					if (sIter->second->GetStarFieldMap() != NULL)
					{
						if (ImGui::ImageButton(sIter->second->GetStarFieldMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 0));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 0));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set MoonMap]");
				if (ImGui::TreeNode("MoonMap"))
				{
					ImGui::Text("Sky MoonMap");

					if (sIter->second->GetMoonMap() != NULL)
					{
						if (ImGui::ImageButton(sIter->second->GetMoonMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 1));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 1));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set GlowMap]");
				if (ImGui::TreeNode("GlowMap"))
				{
					ImGui::Text("Sky GlowMap");

					if (sIter->second->GetGlowMap() != NULL)
					{
						if (ImGui::ImageButton(sIter->second->GetGlowMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 2));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 2));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set CloudMap]");
				if (ImGui::TreeNode("CloudMap"))
				{
					ImGui::Text("Sky CloudMap");

					if (sIter->second->GetCloudMap() != NULL)
					{
						if (ImGui::ImageButton(sIter->second->GetCloudMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 3));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadSkyMapFile, this, sIter->second, placeholders::_1, 3));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Layleigh&Mie]");
				ImGui::Image(sIter->second->GetRayleigh()->SRV(), ImVec2(100, 100));
				ImGui::SameLine();
				ImGui::Image(sIter->second->GetMie()->SRV(), ImVec2(100, 100));
				ImGui::Separator();
				ImGui::Separator();

				float preCloudScale = sIter->second->CloudScale();
				float preCloudSpeed = sIter->second->CloudSpeed();

				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Cloud]");
				ImGui::Text("Cloud Scale");
				ImGui::SliderFloat("##cloudScale", &sIter->second->CloudScale(), 0.0f, 100.0f, "%.5f");
				ImGui::Text("Cloud Speed");
				ImGui::SliderFloat("##cloudSpeed", &sIter->second->CloudSpeed(), 0.0f, 50.0f, "%.5f");

				if (preCloudScale != sIter->second->CloudScale())
					sIter->second->GetEffect()->AsScalar("cloudscale")->SetFloat(sIter->second->CloudScale());

				if (preCloudSpeed != sIter->second->CloudSpeed())
					sIter->second->GetEffect()->AsScalar("speed")->SetFloat(sIter->second->CloudSpeed());

				ImGui::Separator();
				ImGui::Separator();
			}
		}
		//==================================================================================//


		//======================================레인=====================================//
		rainIter rIter = rains.begin();
		for (; rIter != rains.end(); rIter++)
		{
			if (rIter->second->IsPicked())
			{
				rIter->second->ImGuiRender();
				ImGui::Separator();
				ImGui::Separator();
			}
		}
		//================================================================================//
		ImGui::End();
	}
}

void MapTool::DrawHiarachy()
{
	ImGui::Begin("Hiarachy", NULL);
	{

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "========[Game Object]========");
		//=================================모델콤보창===================================//
		if (ImGui::BeginCombo("Objects", String::ToString(selectObjName).c_str()))
		{
			for (size_t i = 0; i < ObjectNames.size(); i++)
			{
				bool isSelected = (selectObjName == ObjectNames[i]);
				if (ImGui::Selectable(String::ToString(ObjectNames[i]).c_str(), isSelected))
				{
					selectObjName = ObjectNames[i];
					CheckSelectObject();
				}
				if (isSelected == true)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		else if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("terrainLoad"))
			{
				IM_ASSERT(payload->DataSize == sizeof(Terrain*));
				Terrain** data = (Terrain**)payload->Data;

				Terrain*copy = new Terrain(**data);
				copy->Initialize();
				copy->Ready();
				copy->Name() = (*data)->Name();

				terrains[(*data)->Name()] = copy;
				shadow->Add(terrains[(*data)->Name()]);
				shadow->Add(terrains[(*data)->Name()]->GetBillboard());
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("skyLoad"))
			{
				IM_ASSERT(payload->DataSize == sizeof(Sky*));
				Sky** data = (Sky**)payload->Data;

				Sky*copy = new Sky(**data);
				copy->Initialize();
				copy->Ready();
				copy->Name() = (*data)->Name();

				skys[(*data)->Name()] = copy;
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("rainLoad"))
			{
				IM_ASSERT(payload->DataSize == sizeof(Rain*));
				Rain** data = (Rain**)payload->Data;

				Rain*copy = new Rain(**data);
				copy->Ready();
				copy->Name() = (*data)->Name();

				rains[(*data)->Name()] = copy;

			}
			ImGui::EndDragDropTarget();
		}
		//===================================================================================//


		//===================================================================================//
		if (ImGui::Button("COPY", ImVec2(95, 20)))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("DELETE", ImVec2(95, 20)))
		{

			terrainIter tIter = terrains.find(selectObjName);
			if (tIter != terrains.end())
			{
				shadow->Erase(tIter->second);
				shadow->Erase(tIter->second->GetBillboard());
				ColliderManager::Get()->Erase(tIter->second->GetBillboard());
				FileManager::Get()->Erase(tIter->second);
				SAFE_DELETE(tIter->second);
				terrains.erase(tIter);
			}

			cubeSkyIter cIter = cubeSkys.find(selectObjName);
			if (cIter != cubeSkys.end())
			{

				SAFE_DELETE(cIter->second);
				cubeSkys.erase(cIter);
			}

			SkyIter sIter = skys.find(selectObjName);
			if (sIter != skys.end())
			{
				FileManager::Get()->Erase(sIter->second);
				SAFE_DELETE(sIter->second);
				skys.erase(sIter);
			}

			rainIter rIter = rains.find(selectObjName);
			if (rIter != rains.end())
			{
				FileManager::Get()->Erase(rIter->second);
				SAFE_DELETE(rIter->second);
				rains.erase(rIter);
			}


			for (size_t i = 0; i < ObjectNames.size(); i++)
			{
				if (ObjectNames[i] == selectObjName)
				{
					ObjectNames.erase(ObjectNames.begin() + i);
					break;
				}
			}
			selectObjName = L"";
		}

		ImGui::Separator();
		ImGui::Separator();

		//==================================================================================//
		if (ImGui::SmallButton("Environment"))
		{
			if (!eType.isShowCombLandScape)eType.isShowCombLandScape = true;
			else eType.isShowCombLandScape = false;
		}

		ImGui::Separator();
		if (eType.isShowCombLandScape)
		{
			//======================================터레인=====================================//
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
			{
				static bool isLandSpapeCheck = false;
				if (ImGui::TreeNode(String::ToString(tIter->second->Name()).c_str()))
				{
					PickedInit();
					tIter->second->SetPickState(true);
					if (selectObjName != tIter->first)
						selectObjName = tIter->first;

					isLandSpapeCheck = true;
					ImGui::TreePop();
				}
				else
				{
					if (isLandSpapeCheck)
					{
						tIter->second->SetPickState(false);
						selectObjName = L"";
						isLandSpapeCheck = false;
					}
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					ImGui::SetDragDropPayload("Terrain", tIter->second, sizeof(Terrain));
					ImGui::Text(String::ToString(tIter->second->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
			//==================================================================================//


			//======================================큐브스카이===================================//
			cubeSkyIter cIter = cubeSkys.begin();
			for (; cIter != cubeSkys.end(); cIter++)
			{
				static bool isCubeSkyCheck = false;
				if (ImGui::TreeNode(String::ToString(cIter->second->Name()).c_str()))
				{
					PickedInit();
					cIter->second->SetPickState(true);
					if (selectObjName != cIter->first)
						selectObjName = cIter->first;

					isCubeSkyCheck = true;
					ImGui::TreePop();
				}
				else
				{
					if (isCubeSkyCheck)
					{
						cIter->second->SetPickState(false);
						selectObjName = L"";
						isCubeSkyCheck = false;
					}
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					ImGui::SetDragDropPayload("CubeSky", cIter->second, sizeof(CubeSky));
					ImGui::Text(String::ToString(cIter->second->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
			//==================================================================================//


			//======================================스카이=======================================//
			SkyIter sIter = skys.begin();
			for (; sIter != skys.end(); sIter++)
			{
				static bool isSkyCheck = false;
				if (ImGui::TreeNode(String::ToString(sIter->second->Name()).c_str()))
				{
					PickedInit();
					sIter->second->SetPickState(true);
					if (selectObjName != sIter->first)
						selectObjName = sIter->first;

					isSkyCheck = true;
					ImGui::TreePop();
				}
				else
				{
					if (isSkyCheck)
					{
						sIter->second->SetPickState(false);
						selectObjName = L"";
						isSkyCheck = false;
					}
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					ImGui::SetDragDropPayload("Sky", sIter->second, sizeof(Sky));
					ImGui::Text(String::ToString(sIter->second->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
			//==================================================================================//


			//======================================레인=========================================//
			rainIter rIter = rains.begin();
			for (; rIter != rains.end(); rIter++)
			{
				static bool isRainCheck = false;
				if (ImGui::TreeNode(String::ToString(rIter->second->Name()).c_str()))
				{
					PickedInit();
					rIter->second->SetPickState(true);
					if (selectObjName != rIter->first)
						selectObjName = rIter->first;

					isRainCheck = true;
					ImGui::TreePop();
				}
				else
				{
					if (isRainCheck)
					{
						rIter->second->SetPickState(false);
						selectObjName = L"";
						isRainCheck = false;
					}
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					ImGui::SetDragDropPayload("Rain", rIter->second, sizeof(Rain));
					ImGui::Text(String::ToString(rIter->second->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
			//===================================================================================//
		}
		ImGui::Separator();
	}
	ImGui::End();
}

void MapTool::CheckSelectObject()
{
	PickedInit();

	terrainIter tIter = terrains.find(selectObjName);
	if (tIter != terrains.end())
	{
		tIter->second->SetPickState(true);
		return;
	}

	cubeSkyIter cIter = cubeSkys.find(selectObjName);
	if (cIter != cubeSkys.end())
	{
		cIter->second->SetPickState(true);
		return;
	}

	SkyIter sIter = skys.find(selectObjName);
	for (; sIter != skys.end(); sIter++)
	{
		sIter->second->SetPickState(true);
		return;
	}

	rainIter rIter = rains.find(selectObjName);
	for (; rIter != rains.end(); rIter++)
	{
		rIter->second->SetPickState(true);
		return;
	}
}

void MapTool::LoadTerrainMapFile(TerrainRender * tree, wstring filename, int type)
{
	tree->ChangeImageMap(filename, type);
}

void MapTool::LoadHeightMapFile(Terrain*terrain, wstring file)
{
	terrain->Desc().HeightMapInit(file, terrain->Desc().HeightScale);

	terrain->GetHeightMap()->Load(terrain->Desc().HeightMap);
	terrain->GetTerrainRender()->Initialize();

	terrain->Ready();
}

void MapTool::LoadBlendMapFile(Terrain*terrain, wstring file, int bNum)
{
	SAFE_DELETE(layerImg[bNum]);

	layerImg[bNum] = new Texture(file);
	terrain->Desc().BlendMapInit(file, bNum);

	terrain->Ready();
}

void MapTool::LoadSkyMapFile(class Sky*sky, wstring file, int type)
{
	switch (type)
	{
		case 0:
		{
			SAFE_DELETE(sky->GetStarFieldMap());

			sky->GetStarFieldMap() = new Texture(file);
			sky->GetEffect()->AsSRV("StarfieldMap")->SetResource(sky->GetStarFieldMap()->SRV());
		}
		break;
		case 1:
		{
			SAFE_DELETE(sky->GetMoonMap());

			sky->GetMoonMap() = new Texture(file);
			sky->GetEffect()->AsSRV("MoonMap")->SetResource(sky->GetMoonMap()->SRV());
		}
		break;
		case 2:
		{
			SAFE_DELETE(sky->GetGlowMap());

			sky->GetGlowMap() = new Texture(file);
			sky->GetEffect()->AsSRV("MoonGlowMap")->SetResource(sky->GetGlowMap()->SRV());
		}
		break;
		case 3:
		{
			SAFE_DELETE(sky->GetCloudMap());

			int perm[] = { 151,160,137,91,90,15,
				131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
			};

			int gradValues[] = {
				1,1,0,
				-1,1,0, 1,-1,0,
				-1,-1,0, 1,0,1,
				-1,0,1, 1,0,-1,
				-1,0,-1, 0,1,1,
				0,-1,1, 0,1,-1,
				0,-1,-1, 1,1,0,
				0,-1,1, -1,1,0,
				0,-1,-1
			};

			sky->GetCloudMap() = new Texture(file);

			vector<D3DXCOLOR> pixels;
			for (int i = 0; i<256; i++)
			{
				for (int j = 0; j<256; j++)
				{
					//int offset = (i * 256 + j);
					int value = perm[(j + perm[i]) & 0xFF];
					D3DXCOLOR color;
					color.r = (float)(gradValues[value & 0x0F] * 64 + 64);
					color.g = (float)(gradValues[value & 0x0F + 1] * 64 + 64);
					color.b = (float)(gradValues[value & 0x0F + 2] * 64 + 64);
					color.a = (float)value;
					pixels.push_back(color);
				}
			}
			sky->GetCloudMap()->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, pixels, true);
			sky->GetEffect()->AsSRV("CloudMap")->SetResource(sky->GetCloudMap()->SRV());
		}
		break;
	}
}

void MapTool::LoadCubeSkyMapFile(CubeSky * sky, wstring file)
{
	sky->CreateSRV(file);
}

void MapTool::CreateComputeTexture()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 2049;
	desc.Height = 2049;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D*texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);
	assert(SUCCEEDED(hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R16_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(texture);
}

void MapTool::CheckSelectType()
{
	if (selectType == L" None")
	{
		mSetType = SET_NONE;
	}
	else if (selectType == L" Detail_Terrain")
	{
		mSetType = SET_DETAIL_TERRAIN;
	}
	else if (selectType == L" Height_Terrain")
	{
		mSetType = SET_HEIGHT_TERRAIN;
	}
	else if (selectType == L" Brush_Terrain")
	{
		mSetType = SET_BRUSH_TERRAIN;
	}
	else if (selectType == L" Blend_Terrain")
	{
		mSetType = SET_BLEND_TERRAIN;
	}
	else if (selectType == L" Fog_Terrain")
	{
		mSetType = SET_FOG_TERRAIN;
	}
	else if (selectType == L" Billboard_Terrain")
	{
		mSetType = SET_BILLBOARD_TERRAIN;
	}
}

void MapTool::TerrainTypeRender(class Terrain*terrain)
{
	ImGui::Separator();
	ImGui::Separator();
	switch (mSetType)
	{
		case SET_NONE:
		{
			wstring textTrans = L"[" + terrain->Name() + L"]translate";
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
			ImGui::Text(String::ToString(textTrans).c_str());
			ImGui::DragFloat3("##trans1", (float*)&terrain->GetTerrainRender()->GetSRT().trans, 0.05f, -128, 128);

			wstring textScale = L"[" + terrain->Name() + L"]scale";
			ImGui::Text(String::ToString(textScale).c_str());
			ImGui::DragFloat3("##scale1", (float*)&terrain->GetTerrainRender()->GetSRT().scale, 0.05f, -10, 10);

			wstring textRotate = L"[" + terrain->Name() + L"]rotate";
			ImGui::Text(String::ToString(textRotate).c_str());
			ImGui::DragFloat3("##rotate1", (float*)&terrain->GetTerrainRender()->GetSRT().rotate, 0.05f, -3.14f, 3.14f);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
			if (ImGui::TreeNode("Diffuse"))
			{
				ImGui::Text("Terrain DiffuseMap");

				if (terrain->Desc().material->GetDiffuseMap() != NULL)
				{
					if (ImGui::ImageButton(terrain->Desc().material->GetDiffuseMap()->SRV(), ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 0));
					}
				}
				else
				{
					if (ImGui::ImageButton(0, ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 0));
					}
				}
				ImGui::SameLine();
				ImGui::TreePop();

				ImGui::Separator();
				ImGui::Text("Diffuse Color");
				D3DXCOLOR diffuseColor = terrain->Desc().material->GetDiffuse();
				ImGui::ColorEdit3("##dcolor", diffuseColor);
				terrain->Desc().material->SetDiffuse(diffuseColor);
			}

			ImGui::Separator();
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
			if (ImGui::TreeNode("Specular"))
			{
				ImGui::Text("Terrain SpecularMap");

				if (terrain->Desc().material->GetSpecularMap() != NULL)
				{
					if (ImGui::ImageButton(terrain->Desc().material->GetSpecularMap()->SRV(), ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 1));
					}
				}
				else
				{
					if (ImGui::ImageButton(0, ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 1));
					}
				}
				ImGui::SameLine();
				ImGui::TreePop();

				ImGui::Separator();
				ImGui::Text("Specular Color");
				D3DXCOLOR specularColor = terrain->Desc().material->GetSpecular();
				ImGui::ColorEdit3("##scolor", specularColor);
				terrain->Desc().material->SetSpecular(specularColor);
				ImGui::Separator();
				ImGui::Text("Shininess");
				float shininess = terrain->Desc().material->GetSpecular().a;
				ImGui::DragFloat("##sness", &shininess, 0.5f, 1, 60);
				D3DXCOLOR specular = terrain->Desc().material->GetSpecular();
				specular.a = shininess;
				terrain->Desc().material->SetSpecular(specular);
			}

			ImGui::Separator();
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
			if (ImGui::TreeNode("NormalMap"))
			{
				ImGui::Text("Terrain NormalMap");

				if (terrain->Desc().material->GetNormalMap() != NULL)
				{
					if (ImGui::ImageButton(terrain->Desc().material->GetNormalMap()->SRV(), ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 2));
					}
				}
				else
				{
					if (ImGui::ImageButton(0, ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 2));
					}
				}
				ImGui::SameLine();
				ImGui::TreePop();
			}
		}
		break;
		case SET_DETAIL_TERRAIN:
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set DetailMap]");
			if (ImGui::TreeNode("DetailMap"))
			{
				ImGui::Text("Terrain DetailMap");

				if (terrain->Desc().material->GetDetailMap() != NULL)
				{
					if (ImGui::ImageButton(terrain->Desc().material->GetDetailMap()->SRV(), ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 3));
					}
				}
				else
				{
					if (ImGui::ImageButton(0, ImVec2(100, 100)))
					{
						Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadTerrainMapFile, this, terrain->GetTerrainRender(), placeholders::_1, 3));
					}
				}
				ImGui::SameLine();
				ImGui::TreePop();
			}
			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[TerrainFrustumInfo]");
			ImGui::Text("FillMode");
			ImGui::SliderInt("##fm", &terrain->GetTerrainRender()->WireFrameMode(), 0, 1);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetDetailInfo]");
			ImGui::Text("DetailIntensity");
			ImGui::DragFloat("##detailIntensity", &terrain->detail.DetailIntensity, 0.01f, 1, 10);
			terrain->Desc().material->GetEffect()->AsScalar("DetailIntensity")->SetFloat(terrain->detail.DetailIntensity);

			ImGui::Text("DepthValue");
			ImGui::DragFloat("##depthValue", &terrain->detail.DetailValue, 0.00001f, 0, 1, "%.4f");
			terrain->Desc().material->GetEffect()->AsScalar("DetailValue")->SetFloat(terrain->detail.DetailValue);

			ImGui::Text("TextureSize");
			ImGui::DragInt("##textureSize", (int*)&terrain->GetTerrainRender()->GetBuffer().TexScale, 1, 1, 64);

		}
		break;
		case SET_HEIGHT_TERRAIN:
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[HeightMap]");

			ImGui::Text("Current HeightMap");
			ImGui::Image(terrain->GetTerrainRender()->GetHeightMapSRV(), ImVec2(100, 100));

			if (ImGui::Button("Load HeightMap"))
			{
				Path::OpenFileDialog(L"파일", L".raw", Textures + L"HeightMap/", bind(&MapTool::LoadHeightMapFile, this, terrain, placeholders::_1));
			}
			ImGui::SameLine();
			if (ImGui::Button("Save HeightMap"))
			{

			}
		}
		break;
		case SET_BRUSH_TERRAIN:
		{
			terrain->LineUpdate();
			D3DXVECTOR3 position, pixelPos;
			bool bPicked = terrain->Picking(&pixelPos, &position);
			if (bPicked)
			{

				terrain->Desc().material->GetEffect()->AsScalar("Type")->SetInt(terrain->brush.Type);
				terrain->Desc().material->GetEffect()->AsVector("Location")->SetFloatVector(position);
				terrain->Desc().material->GetEffect()->AsScalar("Range")->SetInt(terrain->brush.Range);
				terrain->Desc().material->GetEffect()->AsVector("Color")->SetFloatVector(terrain->brush.Color);


				if (Mouse::Get()->Press(2))
				{
					brushEffect->AsScalar("BrushType")->SetInt(terrain->brush.Type);
					brushEffect->AsScalar("BrushRange")->SetInt(terrain->brush.Range);
					brushEffect->AsScalar("BrushPower")->SetInt((int)terrain->brush.BrushPower);
					brushEffect->AsVector("PixelPosition")->SetFloatVector(pixelPos);
					brushEffect->AsScalar("BrushValue")->SetInt((int)mBrushType);
					terrain->GetTerrainRender()->AdjustY(brushEffect, srv, uav);
				}

			}
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[TerrainLine]");
			ImGui::Text("LineType");
			ImGui::SliderInt("##lt", &terrain->tline.LineType, 0, 2);
			ImGui::Text("LineColor");
			ImGui::ColorEdit3("lineColor", terrain->tline.LineColor);
			ImGui::Text("LineSpacing");
			ImGui::SliderInt("##ls", &terrain->tline.Spacing, 1, 10);
			ImGui::Text("LineThickness");
			ImGui::DragFloat("##ltn", &terrain->tline.Thickness, 0.1f, 0.1f, 10);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[TerrainBrush]");
			ImGui::RadioButton("NoneBrush", (int*)&mBrushType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("UpBrush", (int*)&mBrushType, 1);
			ImGui::RadioButton("DownBrush", (int*)&mBrushType, 2);

			ImGui::Text("BrushType");
			ImGui::SliderInt("##bt", &terrain->brush.Type, 0, 2);
			ImGui::Text("BrushColor");
			ImGui::ColorEdit3("##brushColor", terrain->brush.Color);
			ImGui::Text("BrushRange");
			ImGui::SliderInt("##br", &terrain->brush.Range, 1, 100);
			ImGui::Text("BrushPower");
			ImGui::DragFloat("##bp", &terrain->brush.BrushPower, 0.5f, 10, 100);
		}
		break;
		case SET_BLEND_TERRAIN:
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[TerrainBlend]");

			ImGui::RadioButton("Blend1(Middle)", (int*)&mBlendType, BLEND1);
			ImGui::RadioButton("Blend2(Top)", (int*)&mBlendType, BLEND2);
			ImGui::RadioButton("Blend3(Bottom)", (int*)&mBlendType, BLEND3);

			ImGui::Separator();
			if (mBlendType == BLEND1)
			{
				if (ImGui::TreeNode("bMap"))
				{
					ImGui::Text("BlendMap1");

					if (layerImg[0] != NULL)
					{
						if (ImGui::ImageButton(layerImg[0]->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 0));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 0));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();

				ImGui::Checkbox("TurnOffBlend", (bool*)&terrain->Desc().activeBlend.x);
				ImGui::Text("BlendHeight");
				ImGui::DragFloat("##bheight", &terrain->Desc().blendHeight.x, 0.001f, -1, 1);

				terrain->GetTerrainRender()->BlendSet();
			}
			else if (mBlendType == BLEND2)
			{
				if (ImGui::TreeNode("bMap"))
				{
					ImGui::Text("BlendMap2");

					if (layerImg[1] != NULL)
					{
						if (ImGui::ImageButton(layerImg[1]->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 1));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 1));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();

				ImGui::Checkbox("TurnOffBlend", (bool*)&terrain->Desc().activeBlend.y);
				ImGui::Text("BlendHeight");
				ImGui::DragFloat("##bheight", &terrain->Desc().blendHeight.y, 0.001f, -1, 1);
				terrain->GetTerrainRender()->BlendSet();
			}
			else if (mBlendType == BLEND3)
			{
				if (ImGui::TreeNode("bMap"))
				{
					ImGui::Text("BlendMap3");

					if (layerImg[2] != NULL)
					{
						if (ImGui::ImageButton(layerImg[2]->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 2));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&MapTool::LoadBlendMapFile, this, terrain, placeholders::_1, 2));
						}
					}
					ImGui::SameLine();
					ImGui::TreePop();
				}
				ImGui::Separator();

				ImGui::Checkbox("TurnOffBlend", (bool*)&terrain->Desc().activeBlend.z);
				ImGui::Text("BlendHeight");
				ImGui::DragFloat("##bheight", &terrain->Desc().blendHeight.z, 0.001f, -1, 1);
				terrain->GetTerrainRender()->BlendSet();
			}
		}
		break;
		case SET_FOG_TERRAIN:
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetFog]");
			ImGui::DragFloat("FogStart", &terrain->GetTerrainRender()->GetBuffer().FogStart, 10, 0, 600);
			ImGui::DragFloat("FogRange", &terrain->GetTerrainRender()->GetBuffer().FogRange, 10, 0, 600);
			ImGui::ColorEdit3("FogColor", terrain->GetTerrainRender()->GetBuffer().FogColor);
		}
		break;
		case SET_BILLBOARD_TERRAIN:
		{
			terrain->CreateBillboard(shadow, lineEffect);
			terrain->GetBillboard()->ImGuiRender();
			D3DXVECTOR3 pos;
			if (Mouse::Get()->Down(2))
			{
				if (terrain->Picking(NULL, &pos))
				{
					terrain->GetBillboard()->BuildBillboard(pos);
				}
			}
		}
		break;
	}
}

void MapTool::LoadMapData(wstring fileName)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;
	node = matNode->FirstChildElement(); // loadStarMap
	wstring type = String::ToWString(node->GetText());


	if (type == L"Terrain")
	{
		Terrain::InitDesc desc;
		Terrain*terrain = new Terrain(desc);

		terrain->Load(fileName);
		terrain->Initialize();
		terrain->Ready();

		wstring terrainName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = terrainName.find(dot);
		int size = terrainName.size();

		terrainName.erase(index, size);

		terrain->Name() = terrainName;

		loadTerrains.push_back(terrain);
	}
	else if (type == L"Sky")
	{
		Sky*sky = new Sky();

		sky->Load(fileName);
		sky->Initialize();
		sky->Ready();

		wstring skyName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = skyName.find(dot);
		int size = skyName.size();

		skyName.erase(index, size);

		sky->Name() = skyName;

		loadSkys.push_back(sky);
	}
	else if (type == L"Rain")
	{
		Rain*rain = new Rain(D3DXVECTOR3(100, 100, 100), 1000);
		rain->Load(fileName);
		rain->Ready();

		wstring rainName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = rainName.find(dot);
		int size = rainName.size();

		rainName.erase(index, size);

		rain->Name() = rainName;

		loadRains.push_back(rain);
	}
}


