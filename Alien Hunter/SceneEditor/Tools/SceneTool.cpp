#include "stdafx.h"
#include "SceneTool.h"

#include "Environment/Terrain.h"
#include "Environment/TerrainRender.h"
#include "Environment/HeightMap.h"
#include "Environment/CubeSky.h"
#include "Environment/Sky.h"
#include "Environment/Rain.h"
#include "Environment/Shadow.h"
#include "Environment\Billboard.h"
#include "Collider\ColliderBox.h"

#include "Model\ModelClip.h"

#include "Utilities/Xml.h"
#include "Systems/Window.h"
#include "Draw/GizmoGrid.h"
#include "Draw/Gizmo.h"

SceneTool::SceneTool()
{
	grid = new GizmoGrid();

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	gizmo = new Gizmo(identity);

	shadow = new Shadow(2048, 2048);
	shadow->Initialize();

	selectObjName = L"";
	tagNames.push_back(L" Terrains");
	tagNames.push_back(L" Skys");
	tagNames.push_back(L" Rains");
	tagNames.push_back(L" Model");		//일반모델
	tagNames.push_back(L" Charactor");	//에님모델

	lineEffect = new Effect(Effects + L"001_Line.hlsl");
}

SceneTool::~SceneTool()
{
	/*modelIter modelIter = models.begin();
	for (; modelIter != models.end();modelIter++)
	{
		SAFE_DELETE(modelIter->second);
	}
	models.clear();

	animIter aIter = animModels.begin();
	for (; aIter != animModels.end();aIter++)
	{
		SAFE_DELETE(aIter->second);
	}
	animModels.clear();*/

	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end();tIter++)
	{
		SAFE_DELETE(tIter->second);
	}
	terrains.clear();

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
	SAFE_DELETE(gizmo);

	SAFE_DELETE(lineEffect);
}

void SceneTool::Ready()
{
	shadow->Ready();
}

void SceneTool::Render()
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
				if (ImGui::MenuItem("Load Scene"))
				{
					mType = LOAD_SCENE;
				}
				if (ImGui::MenuItem("Save Scene"))
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
				if (ImGui::BeginMenu("Option"))
				{
					if (ImGui::MenuItem("Use Debug"))
					{
						UseDebugLine();
					}
					if (ImGui::MenuItem("Use Gizom"))
					{
						if (useGizmo)
							useGizmo = false;
						else
							useGizmo = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("GameManager"))
			{
				if (ImGui::MenuItem("CollisionManager"))
				{
					if (eType.isCollisionManagerOpen)
						eType.isCollisionManagerOpen = false;
					else
						eType.isCollisionManagerOpen = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	//=====================================================================//
	if (eType.isCollisionManagerOpen)
	{
		ColliderManager::Get()->ImGuiLayerMenu();
	}

	//스카이
	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->Render();

	shadow->Render();

	//레인
	rainIter rIter = rains.begin();
	for (; rIter != rains.end(); rIter++)
		rIter->second->Render();

	InstanceManager::Get()->Render();

	modelIter modelIter = models.begin();
	for (; modelIter != models.end(); modelIter++)
		modelIter->second->Render();

	animIter aIter = animModels.begin();
	for (; aIter != animModels.end(); aIter++)
		aIter->second->Render();

	if (sType == SCENETYPE::EDITSCENE)
	{
		//인스펙터창
		DrawInspector();
		//에셋창
		DrawAsset();
		//하이라키 창
		DrawHiarachy();

		if (useGizmo)
		{
			grid->Render();
			gizmo->Render();
		}
	}
}


void SceneTool::Update()
{
	//=================================FBX추출=================================//
	switch (mType)
	{
		case SceneTool::LOAD_DATA:
		{
			Path::OpenFileDialog(L"파일", L"", Datas, bind(&SceneTool::LoadModelData, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case SceneTool::LOAD_SCENE:
		{
			Path::OpenFileDialog(L"파일", L"", Datas, bind(&SceneTool::LoadScene, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case SceneTool::SAVE_DATA:
		{
			//여기선 파일하나하나 저장할수있게해야함

			char*pathName = savePath;
			string path = pathName;

			UINT dataSize = terrains.size() + skys.size() + rains.size() + models.size() + animModels.size();

			if (path.size()<1)
				FileManager::Get()->SaveScene(L"", dataSize);
			else
				FileManager::Get()->SaveScene(String::ToWString(path), dataSize);

			mType = MENU_NONE;
		}
		break;
	}
	//======================================================================//

	if (sType == SCENETYPE::EDITSCENE)
	{
		if(useGizmo)
			gizmo->Update();

		animIter aIter = animModels.begin();
		for (; aIter != animModels.end(); aIter++)
		{
			aIter->second->Stop();
		}
	}
	else if (sType == SCENETYPE::GAMESCENE)
	{
		//==============================모델====================================//
		animIter aIter = animModels.begin();
		for (; aIter != animModels.end(); aIter++)
		{
			aIter->second->Play();
		}
		//======================================================================//
	}

	//==============================모델====================================//
	modelIter modelIter = models.begin();
	for (; modelIter != models.end(); modelIter++)
	{
		modelIter->second->Update();
		modelIter->second->PickUpdate();
	}
	//======================================================================//


	//==============================모델====================================//
	animIter aIter = animModels.begin();
	for (; aIter != animModels.end(); aIter++)
	{
		aIter->second->Update();
		aIter->second->PickUpdate();
	}
	//======================================================================//

	//터레인
	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		tIter->second->Update();

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

void SceneTool::PreRender()
{
	//스카이
	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->PreRender();

	shadow->PreRender();
}

void SceneTool::PostRender()
{

}


void SceneTool::PickedInit()
{
	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		tIter->second->SetPickState(false);

	SkyIter sIter = skys.begin();
	for (; sIter != skys.end(); sIter++)
		sIter->second->SetPickState(false);

	rainIter rIter = rains.begin();
	for (; rIter != rains.end(); rIter++)
		rIter->second->SetPickState(false);

	modelIter modelIter = models.begin();
	for (; modelIter != models.end();modelIter++)
		modelIter->second->SetPickState(false);

	animIter aIter = animModels.begin();
	for (; aIter != animModels.end();aIter++)
		aIter->second->SetPickState(false);

}

void SceneTool::UseDebugLine()
{
	modelIter modelIter = models.begin();
	for (; modelIter != models.end(); modelIter++)
	{
		for (size_t i = 0; i < modelIter->second->GetColliders().size(); i++)
		{
			if (modelIter->second->GetColliders()[i]->ShowLine())
				modelIter->second->GetColliders()[i]->ShowLine() = false;
			else
				modelIter->second->GetColliders()[i]->ShowLine() = true;
		}
	}
	
	animIter aIter = animModels.begin();
	for (; aIter != animModels.end(); aIter++)
	{
		for (size_t i = 0; i < aIter->second->GetColliders().size(); i++)
		{
			if (aIter->second->GetColliders()[i]->ShowLine())
				aIter->second->GetColliders()[i]->ShowLine() = false;
			else
				aIter->second->GetColliders()[i]->ShowLine() = true;
		}
	}
}


void SceneTool::DrawAsset()
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

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "============[AssetFile]============");

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
							ObjectNames.push_back(loadTerrains[i]->Name());
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
							ObjectNames.push_back(loadSkys[i]->Name());
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
							ObjectNames.push_back(loadRains[i]->Name());
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
		else if (selectTagName == L" Model")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Model Datas]: %d", loadModel.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("model", ImGuiDir_Down))
			{
				if (!aType.showModel)aType.showModel = true;
				else aType.showModel = false;
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
					for (size_t i = 0; i < loadModel.size(); i++)
					{
						if (loadModel[i]->Name() == selectLoadDataName)
						{
							if (loadModel[i]->TypeName() == L"Model")
							{
								GameModel*player = dynamic_cast<GameModel*>(loadModel[i]);
								GameModel*copy = new GameModel(*player);
								
								map<wstring, UINT>::iterator idIter = modelIDs.find(copy->Name());
								wstring idName;
								
								if (idIter != modelIDs.end())
								{
									modelIDs[copy->Name()]++;
									idName = idIter->first + to_wstring(idIter->second);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								else
								{
									modelIDs[copy->Name()] = 0;
									idName = copy->Name() + to_wstring(0);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								copy->Ready();

								models[idName] = copy;
								gizmo->SetSelectionPool(models[idName]);
								FileManager::Get()->DataPush(FileManager::FileType::Model, models[idName]);
								ObjectNames.push_back(idName);
								modelGroupNames[CutNumberString(idName)] = CutNumberString(idName);
							}
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<GameModel*>::iterator miter;
				miter = loadModel.begin();
				for (; miter != loadModel.end(); miter++)
				{
					if ((*miter)->Name() == selectLoadDataName)
					{
						SAFE_DELETE(*miter);
						loadModel.erase(miter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showModel)
			{
				for (size_t i = 0; i < loadModel.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadModel[i]->Name())).c_str()))
					{
						selectLoadDataName = loadModel[i]->Name();
						for (size_t j = 0; j<loadModel[i]->GetModel()->Materials().size(); j++)
							if (ImGui::TreeNode("Material"))
							{
								ImGui::Separator();
								ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Texture Maps");
								ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)",
									loadModel[i]->GetModel()->Materials()[j]->GetDiffuse().r,
									loadModel[i]->GetModel()->Materials()[j]->GetDiffuse().g,
									loadModel[i]->GetModel()->Materials()[j]->GetDiffuse().b);

								ImGui::Text("Diffuse Map");
								if (loadModel[i]->GetModel()->Materials()[j]->GetDiffuseMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadModel[i]->GetModel()->Materials()[j]->GetDiffuseMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();

								ImGui::Text("Specular Color : (%.2f,%.2f,%.2f,%.2f)",
									loadModel[i]->GetModel()->Materials()[j]->GetSpecular().r,
									loadModel[i]->GetModel()->Materials()[j]->GetSpecular().g,
									loadModel[i]->GetModel()->Materials()[j]->GetSpecular().b,
									loadModel[i]->GetModel()->Materials()[j]->GetSpecular().a);
								ImGui::Text("Specular Map");
								if (loadModel[i]->GetModel()->Materials()[j]->GetSpecularMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadModel[i]->GetModel()->Materials()[j]->GetSpecularMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();
								ImGui::Text("Normal Map");
								if (loadModel[i]->GetModel()->Materials()[j]->GetNormalMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadModel[i]->GetModel()->Materials()[j]->GetNormalMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();
								ImGui::TreePop();
							}
						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("modelLoad", &loadModel[i], sizeof(GameModel*));
						ImGui::Text(String::ToString(loadModel[i]->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}
		else if (selectTagName == L" Charactor")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Charactor Datas]: %d", loadCharactor.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("charactor", ImGuiDir_Down))
			{
				if (!aType.showCharactor)aType.showCharactor = true;
				else aType.showCharactor = false;
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
					for (size_t i = 0; i < loadCharactor.size(); i++)
					{
						if (loadCharactor[i]->Name() == selectLoadDataName)
						{
							if (loadCharactor[i]->TypeName() == L"Player")
							{
								Player*player = dynamic_cast<Player*>(loadCharactor[i]);
								Player*copy = new Player(*player);

								map<wstring, UINT>::iterator idIter = modelIDs.find(copy->Name());
								wstring idName;

								if (idIter != modelIDs.end())
								{
									modelIDs[copy->Name()]++;
									idName = idIter->first + to_wstring(idIter->second);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
									copy->IsUpdate() = false;
								}
								else
								{
									modelIDs[copy->Name()] = 0;
									idName = copy->Name() + to_wstring(0);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								copy->Ready();

								animModels[idName] = copy;

								gizmo->SetSelectionPool(animModels[idName]);
								FileManager::Get()->DataPush(FileManager::FileType::Player, animModels[idName]);
								ObjectNames.push_back(idName);

								animGroupNames[CutNumberString(idName)] = CutNumberString(idName);
							}
							else if (loadCharactor[i]->TypeName() == L"NearEnemy")
							{
								NearEnemy*player = dynamic_cast<NearEnemy*>(loadCharactor[i]);
								Enemy*copy = new NearEnemy(*player);

								map<wstring, UINT>::iterator idIter = modelIDs.find(copy->Name());
								wstring idName;

								if (idIter != modelIDs.end())
								{
									modelIDs[copy->Name()]++;
									idName = idIter->first + to_wstring(idIter->second);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								else
								{
									modelIDs[copy->Name()] = 0;
									idName = copy->Name() + to_wstring(0);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								copy->Ready();

								animModels[idName] = copy;

								gizmo->SetSelectionPool(animModels[idName]);
								FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[idName]);
								ObjectNames.push_back(idName);

								animGroupNames[CutNumberString(idName)] = CutNumberString(idName);
							}
							else if (loadCharactor[i]->TypeName() == L"Animation")
							{
								GameAnimator*player = dynamic_cast<GameAnimator*>(loadCharactor[i]);
								GameAnimator*copy = new GameAnimator(*player);

								map<wstring, UINT>::iterator idIter = modelIDs.find(copy->Name());
								wstring idName;

								if (idIter != modelIDs.end())
								{
									modelIDs[copy->Name()]++;
									idName = idIter->first + to_wstring(idIter->second);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								else
								{
									modelIDs[copy->Name()] = 0;
									idName = copy->Name() + to_wstring(0);
									copy->InstanceID() = modelIDs[copy->Name()];
									copy->Name() = idName;
								}
								copy->Ready();

								animModels[idName] = copy;

								gizmo->SetSelectionPool(animModels[idName]);
								FileManager::Get()->DataPush(FileManager::FileType::Animation, animModels[idName]);
								ObjectNames.push_back(idName);

								animGroupNames[CutNumberString(idName)] = CutNumberString(idName);
							}
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<GameAnimator*>::iterator miter;
				miter = loadCharactor.begin();
				for (; miter != loadCharactor.end(); miter++)
				{
					if ((*miter)->Name() == selectLoadDataName)
					{
						SAFE_DELETE(*miter);
						loadCharactor.erase(miter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showCharactor)
			{
				for (size_t i = 0; i < loadCharactor.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadCharactor[i]->Name())).c_str()))
					{
						selectLoadDataName = loadCharactor[i]->Name();
						for (size_t j = 0; j<loadCharactor[i]->GetModel()->Materials().size(); j++)
							if (ImGui::TreeNode("Material"))
							{
								ImGui::Separator();
								ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Texture Maps");
								ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)",
									loadCharactor[i]->GetModel()->Materials()[j]->GetDiffuse().r,
									loadCharactor[i]->GetModel()->Materials()[j]->GetDiffuse().g,
									loadCharactor[i]->GetModel()->Materials()[j]->GetDiffuse().b);

								ImGui::Text("Diffuse Map");
								if (loadCharactor[i]->GetModel()->Materials()[j]->GetDiffuseMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadCharactor[i]->GetModel()->Materials()[j]->GetDiffuseMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();

								ImGui::Text("Specular Color : (%.2f,%.2f,%.2f,%.2f)",
									loadCharactor[i]->GetModel()->Materials()[j]->GetSpecular().r,
									loadCharactor[i]->GetModel()->Materials()[j]->GetSpecular().g,
									loadCharactor[i]->GetModel()->Materials()[j]->GetSpecular().b,
									loadCharactor[i]->GetModel()->Materials()[j]->GetSpecular().a);
								ImGui::Text("Specular Map");
								if (loadCharactor[i]->GetModel()->Materials()[j]->GetSpecularMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadCharactor[i]->GetModel()->Materials()[j]->GetSpecularMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();
								ImGui::Text("Normal Map");
								if (loadCharactor[i]->GetModel()->Materials()[j]->GetNormalMap() == NULL)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
									ImGui::Image(0, ImVec2(0, 0));
								}
								else
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
									ImGui::Image(loadCharactor[i]->GetModel()->Materials()[j]->GetNormalMap()->SRV(), ImVec2(80, 80));
								}
								ImGui::Separator();
								ImGui::TreePop();
							}
						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("charactorLoad", &loadCharactor[i], sizeof(GameModel*));
						ImGui::Text(String::ToString(loadCharactor[i]->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}
	}
	ImGui::End();
}

void SceneTool::DrawInspector()
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
				wstring textTrans = L"[" + tIter->second->Name() + L"]translate";
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
				ImGui::Text(String::ToString(textTrans).c_str());
				ImGui::DragFloat3("##trans1", (float*)&tIter->second->GetTerrainRender()->GetSRT().trans, 0.05f, -128, 128);

				wstring textScale = L"[" + tIter->second->Name() + L"]scale";
				ImGui::Text(String::ToString(textScale).c_str());
				ImGui::DragFloat3("##scale1", (float*)&tIter->second->GetTerrainRender()->GetSRT().scale, 0.05f, -10, 10);

				wstring textRotate = L"[" + tIter->second->Name() + L"]rotate";
				ImGui::Text(String::ToString(textRotate).c_str());
				ImGui::DragFloat3("##rotate1", (float*)&tIter->second->GetTerrainRender()->GetSRT().rotate, 0.05f, -3.14f, 3.14f);

				ImGui::Separator();
				ImGui::Separator();

				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
				if (ImGui::TreeNode("Diffuse"))
				{
					ImGui::Text("Terrain DiffuseMap");

					if (tIter->second->Desc().material->GetDiffuseMap() != NULL)
					{
						ImGui::ImageButton(tIter->second->Desc().material->GetDiffuseMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
					}
					ImGui::SameLine();
					ImGui::TreePop();

					ImGui::Separator();
					ImGui::Text("Diffuse Color");
					D3DXCOLOR diffuseColor = tIter->second->Desc().material->GetDiffuse();
					ImGui::ColorEdit3("##dcolor", diffuseColor);
					tIter->second->Desc().material->SetDiffuse(diffuseColor);
				}

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
				if (ImGui::TreeNode("Specular"))
				{
					ImGui::Text("Terrain SpecularMap");

					if (tIter->second->Desc().material->GetSpecularMap() != NULL)
					{
						ImGui::ImageButton(tIter->second->Desc().material->GetSpecularMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
					}
					ImGui::SameLine();
					ImGui::TreePop();

					ImGui::Separator();
					ImGui::Text("Specular Color");
					D3DXCOLOR specularColor = tIter->second->Desc().material->GetSpecular();
					ImGui::ColorEdit3("##scolor", specularColor);
					tIter->second->Desc().material->SetSpecular(specularColor);
					ImGui::Separator();
					ImGui::Text("Shininess");
					float shininess = tIter->second->Desc().material->GetSpecular().a;
					ImGui::DragFloat("##sness", &shininess, 0.5f, 1, 60);
					D3DXCOLOR specular = tIter->second->Desc().material->GetSpecular();
					specular.a = shininess;
					tIter->second->Desc().material->SetSpecular(specular);
				}

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
				if (ImGui::TreeNode("NormalMap"))
				{
					ImGui::Text("Terrain NormalMap");

					if (tIter->second->Desc().material->GetNormalMap() != NULL)
					{
						ImGui::ImageButton(tIter->second->Desc().material->GetNormalMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
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
						ImGui::ImageButton(sIter->second->GetStarFieldMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
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
						ImGui::ImageButton(sIter->second->GetMoonMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
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
						ImGui::ImageButton(sIter->second->GetGlowMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
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
						ImGui::ImageButton(sIter->second->GetCloudMap()->SRV(), ImVec2(100, 100));
					}
					else
					{
						ImGui::ImageButton(0, ImVec2(100, 100));
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

		//======================================모델=====================================//
		modelIter modelIter = models.begin();
		for (; modelIter != models.end(); modelIter++)
		{
			if (modelIter->second->IsPicked())
			{
				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "==============[%s]==============", String::ToString(modelIter->second->Name()).c_str());

				ImGui::Separator();
				ImGui::Separator();
				//ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
				//ImGui::Text("Center(%.2f,%.2f,%.2f)", sIter->second->GetCenterPos().x, miter->second->GetCenterPos().y, miter->second->GetCenterPos().z);

				//ImGui::Separator();
				//ImGui::Separator();
				wstring textTrans = L"[" + modelIter->second->Name() + L"]translate";
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
				ImGui::Text(String::ToString(textTrans).c_str());
				D3DXVECTOR3 position, prePosition;
				modelIter->second->Position(&position);
				prePosition = position;

				ImGui::DragFloat3("##T", (float*)&position, 0.05f, -500, 500);
				modelIter->second->Position(position);

				wstring textScale = L"[" + modelIter->second->Name() + L"]scale";
				ImGui::Text(String::ToString(textScale).c_str());
				D3DXVECTOR3 scale, preScale;
				modelIter->second->Scale(&scale);
				preScale = scale;

				ImGui::DragFloat3("##S", (float*)&scale, 0.05f, -100, 100);
				modelIter->second->Scale(scale);


				wstring textRotate = L"[" + modelIter->second->Name() + L"]rotate";
				ImGui::Text(String::ToString(textRotate).c_str());
				D3DXVECTOR3 rotate, preRotate;
				modelIter->second->Rotation(&rotate);
				preRotate = rotate;

				ImGui::DragFloat3("##R", (float*)&rotate, 0.05f, -3.14f, 3.14f);
				modelIter->second->Rotation(rotate);

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Materials]");
				ImGui::BulletText("Count : %d", modelIter->second->GetModel()->Materials().size());


				for (size_t i = 0; i < modelIter->second->GetModel()->Materials().size(); i++)
				{
					if (ImGui::TreeNode(String::ToString(modelIter->second->GetModel()->Materials()[i]->Name()).c_str()))
					{
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
						if (ImGui::TreeNode("Diffuse"))
						{
							ImGui::Text("Model Diffuse");

							D3DXCOLOR diffuse = modelIter->second->GetModel()->Materials()[i]->GetDiffuse();

							ImGui::ColorEdit4("##d", diffuse);
							{
								modelIter->second->GetModel()->Materials()[i]->SetDiffuse(diffuse);
							}

							ImGui::Text("Model DiffuseMap");
							if (modelIter->second->GetModel()->Materials()[i]->GetDiffuseMap() != NULL)
							{
								ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetDiffuseMap()->SRV(), ImVec2(100, 100));
							}
							else
							{
								ImGui::ImageButton(0, ImVec2(100, 100));
							}
							ImGui::TreePop();
						}

						ImGui::Separator();
						ImGui::Separator();
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
						if (ImGui::TreeNode("Specular"))
						{
							ImGui::Text("Model Specular");

							D3DXCOLOR specular = modelIter->second->GetModel()->Materials()[i]->GetSpecular();
							ImGui::ColorEdit4("##s", specular);
							{
								modelIter->second->GetModel()->Materials()[i]->SetSpecular(specular);
							}
							ImGui::Text("Shininess");
							ImGui::DragFloat("##shiness", &specular.a, 1, 1, 50);
							{
								modelIter->second->GetModel()->Materials()[i]->SetSpecular(specular);
							}

							ImGui::Text("Model SpecularMap");
							if (modelIter->second->GetModel()->Materials()[i]->GetSpecularMap() != NULL)
							{
								ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetSpecularMap()->SRV(), ImVec2(100, 100));
							}
							else
							{
								ImGui::ImageButton(0, ImVec2(100, 100));
							}
							ImGui::TreePop();
						}

						ImGui::Separator();
						ImGui::Separator();
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
						if (ImGui::TreeNode("NormalMap"))
						{
							ImGui::Text("Model NormalMap");
							for (size_t i = 0; i < modelIter->second->GetModel()->Materials().size(); i++)
							{
								if (modelIter->second->GetModel()->Materials()[i]->GetNormalMap() != NULL)
								{
									ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetNormalMap()->SRV(), ImVec2(100, 100));
								}
								else
								{
									ImGui::ImageButton(0, ImVec2(100, 100));
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
				}



				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");
				

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "Colliders[%d]", modelIter->second->GetColliders().size());
				ImGui::SameLine();
				if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
				{
					if (!eType.isColliderOpen)eType.isColliderOpen = true;
					else eType.isColliderOpen = false;
				}

				if (eType.isColliderOpen)
				{
					for (size_t i = 0; i < modelIter->second->GetColliders().size(); i++)
					{
						if (ImGui::TreeNode((String::ToString(modelIter->second->GetColliders()[i]->Name())).c_str()))
						{
							modelIter->second->GetColliders()[i]->ImguiRender();

							ImGui::TreePop();
						}
						else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
						{
							ImGui::SetDragDropPayload("Collider", &modelIter->second->GetColliders()[i], sizeof(ColliderElement*));
							ImGui::Text(String::ToString(modelIter->second->GetColliders()[i]->Name()).c_str());
							ImGui::EndDragDropSource();
						}
					}
				}

				ImGui::Separator();
				ImGui::Separator();
			}
		}
		//==================================================================================//


		//======================================케릭터=====================================//
		animIter aIter = animModels.begin();
		for (; aIter != animModels.end(); aIter++)
		{
			if (aIter->second->IsPicked())
			{
				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "==============[%s]==============", String::ToString(aIter->second->Name()).c_str());

				ImGui::Separator();
				ImGui::Separator();

				CharactorSetRender(aIter->second);
			}
		}
		//==================================================================================//
		ImGui::End();
	}
}

void SceneTool::DrawHiarachy()
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
		//===================================================================================//


		//===================================================================================//
		if (ImGui::Button("COPY", ImVec2(95, 20)))
		{
			modelIter modelIter = models.find(selectObjName);
			if (modelIter != models.end())
			{
				GameModel*player = dynamic_cast<GameModel*>(modelIter->second);
				GameModel*copy = new GameModel(*player);

				wstring modelName = CutNumberString(copy->Name());
				map<wstring, UINT>::iterator idIter = modelIDs.find(modelName);
				wstring idName;

				if (idIter != modelIDs.end())
				{
					modelIDs[modelName]++;
					idName = modelName + to_wstring(modelIDs[modelName]);
					copy->InstanceID() = modelIDs[modelName];
					copy->Name() = idName;
				}
				copy->Ready();

				models[idName] = copy;
				gizmo->SetSelectionPool(models[idName]);
				FileManager::Get()->DataPush(FileManager::FileType::Model, models[idName]);
			}

			animIter aIter = animModels.find(selectObjName);
			if (aIter != animModels.end())
			{
				if (aIter->second->TypeName() == L"Player")
				{
					Player*player = dynamic_cast<Player*>(aIter->second);
					Player*copy = new Player(*player);

					wstring modelName = CutNumberString(copy->Name());
					map<wstring, UINT>::iterator idIter = modelIDs.find(modelName);
					wstring idName;

					if (idIter != modelIDs.end())
					{
						modelIDs[modelName]++;
						idName = idIter->first + to_wstring(modelIDs[modelName]);
						copy->InstanceID() = modelIDs[modelName];
						copy->Name() = idName;
					}
					copy->Ready();

					animModels[idName] = copy;

					gizmo->SetSelectionPool(animModels[idName]);
					FileManager::Get()->DataPush(FileManager::FileType::Player, animModels[idName]);
				}
				else if (aIter->second->TypeName() == L"NearEnemy")
				{
					NearEnemy*player = dynamic_cast<NearEnemy*>(aIter->second);
					Enemy*copy = new NearEnemy(*player);

					wstring modelName = CutNumberString(copy->Name());
					map<wstring, UINT>::iterator idIter = modelIDs.find(modelName);
					wstring idName;

					if (idIter != modelIDs.end())
					{
						modelIDs[modelName]++;
						idName = idIter->first + to_wstring(modelIDs[modelName]);
						copy->InstanceID() = modelIDs[modelName];
						copy->Name() = idName;
					}
					copy->Ready();

					animModels[idName] = copy;

					gizmo->SetSelectionPool(animModels[idName]);
					FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[idName]);
				}
				else if (aIter->second->TypeName() == L"Animation")
				{
					GameAnimator*player = dynamic_cast<GameAnimator*>(aIter->second);
					GameAnimator*copy = new GameAnimator(*player);

					wstring modelName = CutNumberString(copy->Name());
					map<wstring, UINT>::iterator idIter = modelIDs.find(modelName);
					wstring idName;

					if (idIter != modelIDs.end())
					{
						modelIDs[modelName]++;
						idName = idIter->first + to_wstring(modelIDs[modelName]);
						copy->InstanceID() = modelIDs[modelName];
						copy->Name() = idName;
					}
					copy->Ready();

					animModels[idName] = copy;

					gizmo->SetSelectionPool(animModels[idName]);
					FileManager::Get()->DataPush(FileManager::FileType::Animation, animModels[idName]);
				}
			}
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

			modelIter modelIter = models.find(selectObjName);
			if (modelIter != models.end())
			{
				//shadow->Erase(modelIter->second);
				gizmo->Erase(modelIter->second);
				ColliderManager::Get()->Erase(modelIter->second);
				InstanceManager::Get()->DeleteModel(modelIter->second, modelIter->second->InstanceID(), InstanceManager::InstanceType::MODEL);
				//SAFE_DELETE(modelIter->second);
				models.erase(modelIter);
			}

			animIter aIter = animModels.find(selectObjName);
			if (aIter != animModels.end())
			{
				map<wstring, vector<BoneSphere*>>::iterator boneIter;

				//shadow->Erase(modelIter->second);
				gizmo->Erase(aIter->second);
				ColliderManager::Get()->Erase(aIter->second);
				InstanceManager::Get()->DeleteModel(aIter->second, aIter->second->InstanceID(), InstanceManager::InstanceType::ANIMATION);
				//SAFE_DELETE(aIter->second);
				animModels.erase(aIter);
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
		}
		ImGui::Separator();
		//======================================모델===================================//
		if (ImGui::SmallButton("Models"))
		{
			if (!eType.isShowCombModel)eType.isShowCombModel = true;
			else eType.isShowCombModel = false;
		}

		ImGui::Separator();
		if (eType.isShowCombModel)
		{
			map<wstring, wstring>::iterator groupIter = modelGroupNames.begin();

			for (; groupIter != modelGroupNames.end(); groupIter++)
			{
				if (ImGui::TreeNode(String::ToString(groupIter->second).c_str()))
				{
					modelIter modelIter = models.begin();
					for (; modelIter != models.end(); modelIter++)
					{
						wstring modelName = CutNumberString(modelIter->second->Name());

						if (groupIter->second == modelName)
						{
							if (ImGui::TreeNode(String::ToString(modelIter->second->Name()).c_str()))
							{
								PickedInit();
								modelIter->second->SetPickState(true);
								if (selectObjName != modelIter->first)
									selectObjName = modelIter->first;

								ImGui::TreePop();
							}
							else
							{
								modelIter->second->SetPickState(false);
								//selectObjName = L"";
							}
						}
					}
					ImGui::TreePop();
				}
			}
		}
		//==================================================================================//

		ImGui::Separator();
		//======================================케릭터=======================================//
		if (ImGui::SmallButton("Charactor"))
		{
			if (!eType.isShowCombCharactor)eType.isShowCombCharactor = true;
			else eType.isShowCombCharactor = false;
		}
		ImGui::Separator();

		if (eType.isShowCombCharactor)
		{
			map<wstring, wstring>::iterator groupIter = animGroupNames.begin();
			for (; groupIter != animGroupNames.end(); groupIter++)
			{
				if (ImGui::TreeNode(String::ToString(groupIter->second).c_str()))
				{
					animIter aIter = animModels.begin();
					for (; aIter != animModels.end(); aIter++)
					{
						wstring modelName = CutNumberString(aIter->second->Name());

						if (groupIter->second == modelName)
						{
							if (ImGui::TreeNode(String::ToString(aIter->second->Name()).c_str()))
							{
								PickedInit();
								aIter->second->SetPickState(true);
								if (selectObjName != aIter->first)
									selectObjName = aIter->first;

								ImGui::TreePop();
							}
							else
							{
								aIter->second->SetPickState(false);
								//selectObjName = L"";
							}
						}
					}
					ImGui::TreePop();
				}
			}
		}
		//==================================================================================//
		ImGui::Separator();
	}
	ImGui::End();
}

wstring SceneTool::CutNumberString(wstring name)
{
	string modelName = Path::GetFileName(String::ToString(name));

	for (UINT i = 0; i <= 9; i++)
	{
		if (modelName.find(to_string(i)) != string::npos)
		{
			int dotIndex = modelName.find(to_string(i));
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);
			return String::ToWString(modelName);
		}
	}
	return String::ToWString(modelName);
}

void SceneTool::CheckSelectObject()
{
	PickedInit();

	terrainIter tIter = terrains.find(selectObjName);
	if (tIter != terrains.end())
	{
		tIter->second->SetPickState(true);
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

	modelIter modelIter = models.find(selectObjName);
	if (modelIter != models.end())
	{
		modelIter->second->SetPickState(true);
		return;
	}

	animIter aIter = animModels.begin();
	for (; aIter != animModels.end();aIter++)
	{
		aIter->second->SetPickState(true);
		return;
	}
}


void SceneTool::CharactorSetRender(GameAnimator*anim)
{
	//ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
	//ImGui::Text("Center(%.2f,%.2f,%.2f)", sIter->second->GetCenterPos().x, miter->second->GetCenterPos().y, miter->second->GetCenterPos().z);

	//ImGui::Separator();
	//ImGui::Separator();


	wstring textTrans = L"[" + anim->Name() + L"]translate";
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
	ImGui::Text(String::ToString(textTrans).c_str());
	D3DXVECTOR3 position, prePosition;
	anim->Position(&position);
	prePosition = position;

	ImGui::DragFloat3("##T", (float*)&position, 0.05f, -500, 500);
	anim->Position(position);

	wstring textScale = L"[" + anim->Name() + L"]scale";
	ImGui::Text(String::ToString(textScale).c_str());
	D3DXVECTOR3 scale, preScale;
	anim->Scale(&scale);
	preScale = scale;

	ImGui::DragFloat3("##S", (float*)&scale, 0.05f, -100, 100);
	anim->Scale(scale);


	wstring textRotate = L"[" + anim->Name() + L"]rotate";
	ImGui::Text(String::ToString(textRotate).c_str());
	D3DXVECTOR3 rotate, preRotate;
	anim->Rotation(&rotate);
	preRotate = rotate;
	ImGui::DragFloat3("##R", (float*)&rotate, 0.05f, -3.14f, 3.14f);
	anim->Rotation(rotate);

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Materials]");
	ImGui::BulletText("Count : %d", anim->GetModel()->Materials().size());


	for (size_t i = 0; i < anim->GetModel()->Materials().size(); i++)
	{
		if (ImGui::TreeNode(String::ToString(anim->GetModel()->Materials()[i]->Name()).c_str()))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
			if (ImGui::TreeNode("Diffuse"))
			{
				ImGui::Text("Model Diffuse");

				D3DXCOLOR diffuse = anim->GetModel()->Materials()[i]->GetDiffuse();

				ImGui::ColorEdit4("##d", diffuse);
				{
					anim->GetModel()->Materials()[i]->SetDiffuse(diffuse);
				}

				ImGui::Text("Model DiffuseMap");
				if (anim->GetModel()->Materials()[i]->GetDiffuseMap() != NULL)
				{
					ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetDiffuseMap()->SRV(), ImVec2(100, 100));
				}
				else
				{
					ImGui::ImageButton(0, ImVec2(100, 100));
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
			if (ImGui::TreeNode("Specular"))
			{
				ImGui::Text("Model Specular");

				D3DXCOLOR specular = anim->GetModel()->Materials()[i]->GetSpecular();
				ImGui::ColorEdit4("##s", specular);
				{
					anim->GetModel()->Materials()[i]->SetSpecular(specular);
				}
				ImGui::Text("Shininess");
				ImGui::DragFloat("##shiness", &specular.a, 1, 1, 50);
				{
					anim->GetModel()->Materials()[i]->SetSpecular(specular);
				}

				ImGui::Text("Model SpecularMap");
				if (anim->GetModel()->Materials()[i]->GetSpecularMap() != NULL)
				{
					ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetSpecularMap()->SRV(), ImVec2(100, 100));
				}
				else
				{
					ImGui::ImageButton(0, ImVec2(100, 100));
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
			if (ImGui::TreeNode("NormalMap"))
			{
				ImGui::Text("Model NormalMap");

				if (anim->GetModel()->Materials()[i]->GetNormalMap() != NULL)
				{
					ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetNormalMap()->SRV(), ImVec2(100, 100));
				}
				else
				{
					ImGui::ImageButton(0, ImVec2(100, 100));
				}
				ImGui::TreePop();

			}
			ImGui::TreePop();
		}
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");

	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Collider Count[%d]", anim->GetColliders().size());
	ImGui::SameLine();
	if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
	{
		if (!eType.isColliderOpen)eType.isColliderOpen = true;
		else eType.isColliderOpen = false;
	}

	if (eType.isColliderOpen)
	{
		for (size_t i = 0; i < anim->GetColliders().size(); i++)
		{
			if (ImGui::TreeNode((String::ToString(anim->GetColliders()[i]->Name())).c_str()))
			{
				anim->GetColliders()[i]->ImguiRender();

				ImGui::TreePop();
			}
			else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
			{
				ImGui::SetDragDropPayload("Collider", &anim->GetColliders()[i], sizeof(ColliderElement*));
				ImGui::Text(String::ToString(anim->GetColliders()[i]->Name()).c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
	ImGui::Separator();
	ImGui::Separator();

	static bool isWeaponOpen = false;
	if (anim->TypeName() == L"Player")
	{
		Player*player = dynamic_cast<Player*>(anim);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Weapons]");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Weapon Count[%d]", player->GetWeapons().size());
		ImGui::SameLine();
		if (ImGui::ArrowButton("Weapons", ImGuiDir_Down))
		{
			if (!isWeaponOpen)isWeaponOpen = true;
			else isWeaponOpen = false;
		}

		if (isWeaponOpen)
		{
			for (size_t i = 0; i < player->GetWeapons().size(); i++)
			{
				if (ImGui::TreeNode((String::ToString(player->GetWeapons()[i]->Name())).c_str()))
				{
					wstring textTrans = L"[" + player->GetWeapons()[i]->Name() + L"]translate";
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
					ImGui::Text(String::ToString(textTrans).c_str());
					D3DXVECTOR3 position, prePosition;
					player->GetWeapons()[i]->Position(&position);
					prePosition = position;

					ImGui::DragFloat3("##T", (float*)&position, 0.05f, -500, 500);
					player->GetWeapons()[i]->Position(position);

					wstring textScale = L"[" + player->GetWeapons()[i]->Name() + L"]scale";
					ImGui::Text(String::ToString(textScale).c_str());
					D3DXVECTOR3 scale, preScale;
					player->GetWeapons()[i]->Scale(&scale);
					preScale = scale;

					ImGui::DragFloat3("##S", (float*)&scale, 0.05f, -100, 100);
					player->GetWeapons()[i]->Scale(scale);


					wstring textRotate = L"[" + player->GetWeapons()[i]->Name() + L"]rotate";
					ImGui::Text(String::ToString(textRotate).c_str());
					D3DXVECTOR3 rotate, preRotate;
					player->GetWeapons()[i]->Rotation(&rotate);
					preRotate = rotate;
					ImGui::DragFloat3("##R", (float*)&rotate, 0.05f, -3.14f, 3.14f);
					player->GetWeapons()[i]->Rotation(rotate);

					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Materials]");
					ImGui::BulletText("Count : %d", player->GetWeapons()[i]->GetModel()->Materials().size());


					for (size_t j = 0;j < player->GetWeapons()[i]->GetModel()->Materials().size(); j++)
					{
						if (ImGui::TreeNode(String::ToString(player->GetWeapons()[i]->GetModel()->Materials()[j]->Name()).c_str()))
						{
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
							if (ImGui::TreeNode("Diffuse"))
							{
								ImGui::Text("Model Diffuse");

								D3DXCOLOR diffuse = player->GetWeapons()[i]->GetModel()->Materials()[j]->GetDiffuse();

								ImGui::ColorEdit4("##d", diffuse);
								{
									player->GetWeapons()[i]->GetModel()->Materials()[j]->SetDiffuse(diffuse);
								}

								ImGui::Text("Model DiffuseMap");
								if (player->GetWeapons()[i]->GetModel()->Materials()[j]->GetDiffuseMap() != NULL)
								{
									ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetDiffuseMap()->SRV(), ImVec2(100, 100));
								}
								else
								{
									ImGui::ImageButton(0, ImVec2(100, 100));
								}
								ImGui::TreePop();
							}

							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
							if (ImGui::TreeNode("Specular"))
							{
								ImGui::Text("Model Specular");

								D3DXCOLOR specular = player->GetWeapons()[i]->GetModel()->Materials()[j]->GetSpecular();
								ImGui::ColorEdit4("##s", specular);
								{
									player->GetWeapons()[i]->GetModel()->Materials()[j]->SetSpecular(specular);
								}
								ImGui::Text("Shininess");
								ImGui::DragFloat("##shiness", &specular.a, 1, 1, 50);
								{
									player->GetWeapons()[i]->GetModel()->Materials()[j]->SetSpecular(specular);
								}

								ImGui::Text("Model SpecularMap");
								if (player->GetWeapons()[i]->GetModel()->Materials()[j]->GetSpecularMap() != NULL)
								{
									ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetSpecularMap()->SRV(), ImVec2(100, 100));
								}
								else
								{
									ImGui::ImageButton(0, ImVec2(100, 100));
								}
								ImGui::TreePop();
							}
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
							if (ImGui::TreeNode("NormalMap"))
							{
								ImGui::Text("Model NormalMap");

								if (player->GetWeapons()[i]->GetModel()->Materials()[j]->GetNormalMap() != NULL)
								{
									ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetNormalMap()->SRV(), ImVec2(100, 100));
								}
								else
								{
									ImGui::ImageButton(0, ImVec2(100, 100));
								}
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}
					}
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");

					static bool isWeaponCollider = false;
					ImGui::TextColored(ImVec4(1, 1, 1, 1), "Collider Count[%d]", player->GetWeapons()[i]->GetColliders().size());
					ImGui::SameLine();
					if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
					{
						if (!isWeaponCollider)isWeaponCollider = true;
						else isWeaponCollider = false;
					}

					if (isWeaponCollider)
					{
						for (size_t j = 0; j < player->GetWeapons()[i]->GetColliders().size(); j++)
						{
							if (ImGui::TreeNode((String::ToString(player->GetWeapons()[i]->GetColliders()[j]->Name())).c_str()))
							{
								player->GetWeapons()[i]->GetColliders()[j]->ImguiRender();

								ImGui::TreePop();
							}
						}
					}
					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::Separator();
	ImGui::Separator();
}

void SceneTool::LoadModelData(wstring fileName)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;
	node = matNode->FirstChildElement(); // 이름
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
	else if (type == L"Player")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Player*player = new Player(
			0,
			meshName,
			materialName,
			lineEffect);

		player->LoadStart() = true;
		player->Load(fileName);
		player->Ready();

		wstring playerName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = playerName.find(dot);
		int size = playerName.size();

		playerName.erase(index, size);

		player->Name() = playerName;

		loadCharactor.push_back(player);
	}
	else if (type == L"NearEnemy")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Enemy*player = new NearEnemy(
			0,
			meshName,
			materialName,
			lineEffect);

		player->LoadStart() = true;
		player->Load(fileName);
		player->Ready();

		wstring playerName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = playerName.find(dot);
		int size = playerName.size();

		playerName.erase(index, size);

		player->TypeName() = L"NearEnemy";
		player->Name() = playerName;

		loadCharactor.push_back(player);
	}
	else if (type == L"Boss")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Boss*player = new Boss(
			0,
			meshName,
			materialName,
			lineEffect);

		player->LoadStart() = true;
		player->Load(fileName);
		player->Ready();

		wstring playerName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = playerName.find(dot);
		int size = playerName.size();

		playerName.erase(index, size);

		player->TypeName() = L"Boss";
		player->Name() = playerName;

		loadCharactor.push_back(player);
	}
	else if (type == L"Model")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		GameModel*model = new GameModel(
			0,
			meshName,
			materialName,
			lineEffect);

		model->LoadStart() = true;
		model->Load(fileName);
		model->Ready();

		wstring playerName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = playerName.find(dot);
		int size = playerName.size();

		playerName.erase(index, size);

		model->Name() = playerName;

		loadModel.push_back(model);
	}
	else if (type == L"Animation")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		GameAnimator*model = new GameAnimator(
			0,
			meshName,
			materialName,
			lineEffect);

		model->LoadStart() = true;
		model->Load(fileName);
		model->Ready();

		wstring playerName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = playerName.find(dot);
		int size = playerName.size();

		playerName.erase(index, size);

		model->Name() = playerName;

		loadCharactor.push_back(model);
	}
}

void SceneTool::LoadScene(wstring sceneName)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = sceneName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement(); // SceneName
	wstring name = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // datasize
	UINT dataSize = node->IntText();

	for (UINT i = 0; i < dataSize; i++)
	{
		node = node->NextSiblingElement(); // name
		wstring name = String::ToWString(node->GetText());

		if (name == L"Terrain")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Terrain::InitDesc desc;
			Terrain*terrain = new Terrain(desc);

			terrain->Load(dataFile);
			terrain->Initialize();
			terrain->Ready();

			terrain->Name() = L"Terrain";
			terrains[terrain->Name()] = terrain;

			shadow->Add(terrains[terrain->Name()]);
			shadow->Add(terrains[terrain->Name()]->GetBillboard());
			FileManager::Get()->DataPush(FileManager::FileType::Terrain, terrains[terrain->Name()]);
			ObjectNames.push_back(terrain->Name());
		}
		else if (name == L"Rain")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Rain*rain = new Rain(D3DXVECTOR3(100, 100, 100), 1000);

			rain->Load(dataFile);
			rain->Ready();
			
			rain->Name() = L"Rain";
			rains[rain->Name()] = rain;
			FileManager::Get()->DataPush(FileManager::FileType::Rain, rains[rain->Name()]);
			ObjectNames.push_back(rain->Name());
		}
		else if (name == L"Sky")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Sky*sky = new Sky();
			sky->Load(dataFile);
			sky->Initialize();
			sky->Ready();

			sky->Name() = L"Sky";
			skys[sky->Name()] = sky;
			FileManager::Get()->DataPush(FileManager::FileType::Sky, skys[sky->Name()]);
			ObjectNames.push_back(sky->Name());
		}
		else if (name == L"Model")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			map<wstring, UINT>::iterator dIiter = modelIDs.find(String::ToWString(modelName));

			if (dIiter != modelIDs.end())
				modelIDs[String::ToWString(modelName)]++;
			else
				modelIDs[String::ToWString(modelName)] = 0;

			modelGroupNames[String::ToWString(modelName)] = String::ToWString(modelName);

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			GameModel*transModel = new GameModel(modelIDs[String::ToWString(modelName)], materialFile, meshFile, lineEffect);
			transModel->Name() = String::ToWString(modelName) + to_wstring(modelIDs[String::ToWString(modelName)]);

			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->Ready();

			models[transModel->Name()] = transModel;
			gizmo->SetSelectionPool(models[transModel->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Model, models[transModel->Name()]);
			ObjectNames.push_back(transModel->Name());
		}
		else if (name == L"Animation")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			map<wstring, UINT>::iterator dIiter = modelIDs.find(String::ToWString(modelName));

			if (dIiter != modelIDs.end())
				modelIDs[String::ToWString(modelName)]++;
			else
				modelIDs[String::ToWString(modelName)] = 0;

			animGroupNames[String::ToWString(modelName)] = String::ToWString(modelName);

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			GameAnimator*transModel = new GameAnimator(modelIDs[String::ToWString(modelName)], materialFile, meshFile, lineEffect);
			transModel->Name() = String::ToWString(modelName) + to_wstring(modelIDs[String::ToWString(modelName)]);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			transModel->Ready();

			animModels[transModel->Name()] = transModel;
			gizmo->SetSelectionPool(animModels[transModel->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Animation, animModels[transModel->Name()]);
			ObjectNames.push_back(transModel->Name());
		}
		else if (name == L"Player")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			map<wstring, UINT>::iterator dIiter = modelIDs.find(String::ToWString(modelName));

			if (dIiter != modelIDs.end())
				modelIDs[String::ToWString(modelName)]++;
			else
				modelIDs[String::ToWString(modelName)] = 0;

			animGroupNames[String::ToWString(modelName)] = String::ToWString(modelName);

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			Player*transModel = new Player(modelIDs[String::ToWString(modelName)], materialFile, meshFile, lineEffect);
			transModel->Name() = String::ToWString(modelName) + to_wstring(modelIDs[String::ToWString(modelName)]);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			transModel->Ready();

			animModels[transModel->Name()] = transModel;
			gizmo->SetSelectionPool(animModels[transModel->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Player, animModels[transModel->Name()]);
			ObjectNames.push_back(transModel->Name());
		}
		else if (name == L"NearEnemy")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			map<wstring, UINT>::iterator dIiter = modelIDs.find(String::ToWString(modelName));

			if (dIiter != modelIDs.end())
				modelIDs[String::ToWString(modelName)]++;
			else
				modelIDs[String::ToWString(modelName)] = 0;

			animGroupNames[String::ToWString(modelName)] = String::ToWString(modelName);

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			Enemy*transModel = new NearEnemy(modelIDs[String::ToWString(modelName)], materialFile, meshFile, lineEffect);
			transModel->Name() = String::ToWString(modelName) + to_wstring(modelIDs[String::ToWString(modelName)]);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			transModel->Ready();

			animModels[transModel->Name()] = transModel;
			gizmo->SetSelectionPool(animModels[transModel->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[transModel->Name()]);
			ObjectNames.push_back(transModel->Name());
		}
	}

	SAFE_DELETE(document);
}





