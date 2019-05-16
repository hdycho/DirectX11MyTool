#include "stdafx.h"
#include "ObjectTool.h"
#include "Environment\Shadow.h"
#include "Model\ModelClip.h"

#include "Utilities/Xml.h"
#include "Systems/Window.h"
#include "Draw/GizmoGrid.h"
#include "Draw/Gizmo.h"
#include "Fbx\FbxLoader.h"

ObjectTool::ObjectTool()
{
	grid = new GizmoGrid();

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	gizmo = new Gizmo(identity);
	
	shadow = new Shadow(2048, 2048);
	shadow->Initialize();

	selectObjName = L"";
	tagNames.push_back(L" Mesh");
	tagNames.push_back(L" Model");		//일반모델
	tagNames.push_back(L" Charactor");	//에님모델

	selectColliderName = L"";
	colliderNames.push_back(L" Box");
	colliderNames.push_back(L" Line");
	colliderNames.push_back(L" Sphere");

	AnimType = Model;

	animName = L"";

	meshMaterial = new Material(Effects + L"012_Mesh&Model.hlsl");
	lineEffect = new Effect(Effects + L"001_Line.hlsl");
	trailEffect = new Effect(Effects + L"021_TrailRender.hlsl");
}

ObjectTool::~ObjectTool()
{
	meshIter mIter = meshes.begin();
	for (; mIter != meshes.end();mIter++)
	{
		SAFE_DELETE(mIter->second);
	}
	meshes.clear();

	modelIter modelIter = models.begin();
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
	animModels.clear();

	SAFE_DELETE(shadow);
	SAFE_DELETE(grid);
	SAFE_DELETE(gizmo);

	map<wstring, vector<BoneSphere*>>::iterator boneIter = boneMeshes.begin();

	for (; boneIter != boneMeshes.end(); boneIter++)
	{
		for (size_t i = 0; i < boneIter->second.size(); i++)
		{
			SAFE_DELETE(boneIter->second[i]);
		}
		boneIter->second.clear();
	}
	boneMeshes.clear();

	/*for (Mesh*mesh : loadMeshes)
	SAFE_DELETE(mesh);

	for (GameModel*mesh : loadModel)
	SAFE_DELETE(mesh);

	for (GameAnimator*mesh : loadCharactor)
	SAFE_DELETE(mesh);*/

	SAFE_DELETE(lineEffect);
	SAFE_DELETE(trailEffect);
	SAFE_DELETE(meshMaterial);
}

void ObjectTool::Ready()
{
	shadow->Ready();
}

void ObjectTool::Render()
{
	//================================기본창================================//
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Origin Data"))
				{
					if (ImGui::MenuItem("Export Data"))
					{
						mType = EXPORT_DATA;
					}
					if (ImGui::MenuItem("Load Model")) // 씬에바로생성되게
					{
						mType = LOAD_MODEL;
					}
					if (ImGui::BeginMenu("Load Animation")) // 이건에셋으로넘어가게
					{
						if (ImGui::MenuItem("AnimModel")) // 이건에셋으로넘어가게
						{
							mType = LOAD_ANIMATION;
						}
						if (ImGui::MenuItem("Player")) // 이건에셋으로넘어가게
						{
							mType = LOAD_PLAYER;
						}
						if (ImGui::MenuItem("Enemy1"))
						{
							mType = LOAD_ENEMY1;
						}
						if (ImGui::MenuItem("Boss"))
						{
							mType = LOAD_BOSS;
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
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
				if (ImGui::BeginMenu("Meshes"))
				{
					if (ImGui::MenuItem("Cube"))
					{
						static int cubeNum = 0;

						meshes[L"Cube" + to_wstring(cubeNum)] = new MeshCube(meshMaterial, 1, 1, 1, lineEffect);
						meshes[L"Cube" + to_wstring(cubeNum)]->Position(0, 0, 0);
						meshes[L"Cube" + to_wstring(cubeNum)]->Name() = L"Cube" + to_wstring(cubeNum);

						shadow->Add(meshes[L"Cube" + to_wstring(cubeNum)]);
						gizmo->SetSelectionPool(meshes[L"Cube" + to_wstring(cubeNum)]);
						ColliderManager::Get()->InputObject(meshes[L"Cube" + to_wstring(cubeNum)]);

						ObjectNames.push_back(L"Cube" + to_wstring(cubeNum));
						cubeNum++;
					}
					if (ImGui::MenuItem("Cylinder"))
					{
						static int cylinderNum = 0;

						meshes[L"Cylinder" + to_wstring(cylinderNum)] = new MeshCylinder(meshMaterial, 1, 1, 4, 50, 50, lineEffect);
						meshes[L"Cylinder" + to_wstring(cylinderNum)]->Position(0, 0, 0);
						meshes[L"Cylinder" + to_wstring(cylinderNum)]->Name() = L"Cylinder" + to_wstring(cylinderNum);

						shadow->Add(meshes[L"Cylinder" + to_wstring(cylinderNum)]);
						gizmo->SetSelectionPool(meshes[L"Cylinder" + to_wstring(cylinderNum)]);
						ColliderManager::Get()->InputObject(meshes[L"Cylinder" + to_wstring(cylinderNum)]);

						ObjectNames.push_back(L"Cylinder" + to_wstring(cylinderNum));
						cylinderNum++;
					}
					if (ImGui::MenuItem("Grid"))
					{
						static int gridNum = 0;

						meshes[L"Grid" + to_wstring(gridNum)] = new MeshGrid(meshMaterial, 100, 100, 100, 100, 1.0f, 1.0f, lineEffect);
						meshes[L"Grid" + to_wstring(gridNum)]->Position(0, 0, 0);
						meshes[L"Grid" + to_wstring(gridNum)]->Name() = L"Grid" + to_wstring(gridNum);

						shadow->Add(meshes[L"Grid" + to_wstring(gridNum)]);
						gizmo->SetSelectionPool(meshes[L"Grid" + to_wstring(gridNum)]);
						ColliderManager::Get()->InputObject(meshes[L"Grid" + to_wstring(gridNum)]);

						ObjectNames.push_back(L"Grid" + to_wstring(gridNum));
						gridNum++;
					}
					if (ImGui::MenuItem("Quad"))
					{
						static int quadNum = 0;

						meshes[L"Quad" + to_wstring(quadNum)] = new MeshQuad(meshMaterial, 5, 5, lineEffect);
						meshes[L"Quad" + to_wstring(quadNum)]->Position(0, 0, 0);
						meshes[L"Quad" + to_wstring(quadNum)]->Name() = L"Quad" + to_wstring(quadNum);

						shadow->Add(meshes[L"Quad" + to_wstring(quadNum)]);
						gizmo->SetSelectionPool(meshes[L"Quad" + to_wstring(quadNum)]);
						ColliderManager::Get()->InputObject(meshes[L"Quad" + to_wstring(quadNum)]);

						ObjectNames.push_back(L"Quad" + to_wstring(quadNum));
						quadNum++;
					}
					if (ImGui::MenuItem("Sphere"))
					{
						static int sphereNum = 0;

						meshes[L"Sphere" + to_wstring(sphereNum)] = new MeshSphere(meshMaterial, 1, 10, 10, lineEffect);
						meshes[L"Sphere" + to_wstring(sphereNum)]->Position(0, 0, 0);
						meshes[L"Sphere" + to_wstring(sphereNum)]->Name() = L"Sphere" + to_wstring(sphereNum);

						shadow->Add(meshes[L"Sphere" + to_wstring(sphereNum)]);
						gizmo->SetSelectionPool(meshes[L"Sphere" + to_wstring(sphereNum)]);
						ColliderManager::Get()->InputObject(meshes[L"Sphere" + to_wstring(sphereNum)]);

						ObjectNames.push_back(L"Sphere" + to_wstring(sphereNum));
						sphereNum++;
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

		grid->Render();

		if (AnimType != Animation)
			gizmo->Render();
	}

	//===========================오브젝트랜더=================================//
	shadow->Render();
	//======================================================================//
}


void ObjectTool::Update()
{
	//=================================FBX추출=================================//
	switch (mType)
	{
		case ObjectTool::EXPORT_DATA:
		{
			Path::OpenFileDialog(L"파일", L"", Assets, bind(&ObjectTool::ExportFile, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_MODEL:
		{
			Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadModel, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_ANIMATION:
		{
			Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadCharactor, this, placeholders::_1, LOAD_ANIMATION));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_PLAYER:
		{
			Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadCharactor, this, placeholders::_1, LOAD_PLAYER));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_ENEMY1:
		{
			Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadCharactor, this, placeholders::_1, LOAD_ENEMY1));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_BOSS:
		{
			Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadCharactor, this, placeholders::_1, LOAD_ENEMY1));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::LOAD_DATA:
		{
			Path::OpenFileDialog(L"파일", L"", Datas, bind(&ObjectTool::LoadModelData, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case ObjectTool::SAVE_DATA:
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


	//==============================메쉬====================================//
	meshIter mIter = meshes.begin();
	for (; mIter != meshes.end(); mIter++)
		mIter->second->PickUpdate();
	//======================================================================//


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

	if (sType == SCENETYPE::EDITSCENE)
	{
		if (AnimType != Animation)
			gizmo->Update();
	}

	shadow->Update();
}

void ObjectTool::PreRender()
{

	shadow->PreRender();
}

void ObjectTool::PostRender()
{
	PostRenderBoneName();
}


void ObjectTool::PickedInit()
{
	meshIter mIter = meshes.begin();
	for (; mIter != meshes.end(); mIter++)
		mIter->second->SetPickState(false);

}

void ObjectTool::DrawAsset()
{
	ImGui::Begin("Asset", NULL);
	{
		ImGui::Separator();
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Export Type]");
		ImGui::RadioButton("Export Mesh", (int*)&eType.isLoadMesh, 1);
		ImGui::RadioButton("Export Anim", (int*)&eType.isLoadMesh, 0);
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
			for (size_t i = 0; i < tagNames.size(); i++)
			{
				bool isSelected = (selectTagName == tagNames[i]);
				if (ImGui::Selectable(String::ToString(tagNames[i]).c_str(), isSelected))
				{
					selectTagName = tagNames[i];
				}
				if (isSelected == true)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "============[AssetFile]============");

		if (selectTagName == L" Mesh")
		{
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Mesh Datas]: %d", loadMeshes.size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("mesh", ImGuiDir_Down))
			{
				if (!aType.showMesh)aType.showMesh = true;
				else aType.showMesh = false;
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
					for (size_t i = 0; i < loadMeshes.size(); i++)
					{
						if (loadMeshes[i]->Name() == selectLoadDataName)
						{
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("PopData"))
			{
				vector<Mesh*>::iterator miter;
				miter = loadMeshes.begin();
				for (; miter != loadMeshes.end(); miter++)
				{
					if ((*miter)->Name() == selectLoadDataName)
					{
						SAFE_DELETE(*miter);
						loadMeshes.erase(miter);
						break;
					}
				}
				selectLoadDataName = L"";
			}

			ImGui::Separator();
			if (aType.showMesh)
			{
				for (size_t i = 0; i < loadMeshes.size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(loadMeshes[i]->Name())).c_str()))
					{
						selectLoadDataName = loadMeshes[i]->Name();
						if (ImGui::TreeNode("Material"))
						{
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Texture Maps");
							ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)",
								loadMeshes[i]->GetMaterial()->GetDiffuse().r,
								loadMeshes[i]->GetMaterial()->GetDiffuse().g,
								loadMeshes[i]->GetMaterial()->GetDiffuse().b);

							ImGui::Text("Diffuse Map");
							if (loadMeshes[i]->GetMaterial()->GetDiffuseMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadMeshes[i]->GetMaterial()->GetDiffuseMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();

							ImGui::Text("Specular Color : (%.2f,%.2f,%.2f,%.2f)",
								loadMeshes[i]->GetMaterial()->GetSpecular().r,
								loadMeshes[i]->GetMaterial()->GetSpecular().g,
								loadMeshes[i]->GetMaterial()->GetSpecular().b,
								loadMeshes[i]->GetMaterial()->GetSpecular().a);
							ImGui::Text("Specular Map");
							if (loadMeshes[i]->GetMaterial()->GetSpecularMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadMeshes[i]->GetMaterial()->GetSpecularMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::Text("Normal Map");
							if (loadMeshes[i]->GetMaterial()->GetNormalMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(loadMeshes[i]->GetMaterial()->GetNormalMap()->SRV(), ImVec2(80, 80));
							}
							ImGui::Separator();
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("meshLoad", &loadMeshes[i], sizeof(Mesh*));
						ImGui::Text(String::ToString(loadMeshes[i]->Name()).c_str());
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
								copy->Ready();

								models[loadModel[i]->Name()] = copy;
								gizmo->SetSelectionPool(models[loadModel[i]->Name()]);
								FileManager::Get()->DataPush(FileManager::FileType::Model, models[loadModel[i]->Name()]);
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
								copy->Ready();

								animModels[loadCharactor[i]->Name()] = copy;

								CreateBone(animModels[loadCharactor[i]->Name()]);

								gizmo->SetSelectionPool(animModels[loadCharactor[i]->Name()]);
								FileManager::Get()->DataPush(FileManager::FileType::Player, animModels[loadCharactor[i]->Name()]);
							}
							else if (loadCharactor[i]->TypeName() == L"NearEnemy")
							{
								NearEnemy*player = dynamic_cast<NearEnemy*>(loadCharactor[i]);
								Enemy*copy = new NearEnemy(*player);
								copy->Ready();

								animModels[loadCharactor[i]->Name()] = copy;

								CreateBone(animModels[loadCharactor[i]->Name()]);

								gizmo->SetSelectionPool(animModels[loadCharactor[i]->Name()]);
								FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[loadCharactor[i]->Name()]);
							}
							else if (loadCharactor[i]->TypeName() == L"Boss")
							{
								Boss*player = dynamic_cast<Boss*>(loadCharactor[i]);
								Enemy*copy = new Boss(*player);
								copy->Ready();

								animModels[loadCharactor[i]->Name()] = copy;

								CreateBone(animModels[loadCharactor[i]->Name()]);

								gizmo->SetSelectionPool(animModels[loadCharactor[i]->Name()]);
								FileManager::Get()->DataPush(FileManager::FileType::Boss, animModels[loadCharactor[i]->Name()]);
							}
							else if (loadCharactor[i]->TypeName() == L"Animation")
							{
								GameAnimator*player = dynamic_cast<GameAnimator*>(loadCharactor[i]);
								GameAnimator*copy = new GameAnimator(*player);
								copy->Ready();

								animModels[loadCharactor[i]->Name()] = copy;

								CreateBone(animModels[loadCharactor[i]->Name()]);

								gizmo->SetSelectionPool(animModels[loadCharactor[i]->Name()]);
								FileManager::Get()->DataPush(FileManager::FileType::Animation, animModels[loadCharactor[i]->Name()]);
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

void ObjectTool::DrawInspector()
{
	ImGui::Begin("Inspector", NULL);
	{
		//======================================메쉬=====================================//
		meshIter mIter = meshes.begin();
		for (; mIter != meshes.end(); mIter++)
		{
			if (mIter->second->IsPicked())
			{
				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "==============[%s]==============", String::ToString(mIter->second->Name()).c_str());

				ImGui::Separator();
				ImGui::Separator();
				//ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
				//ImGui::Text("Center(%.2f,%.2f,%.2f)", sIter->second->GetCenterPos().x, miter->second->GetCenterPos().y, miter->second->GetCenterPos().z);

				//ImGui::Separator();
				//ImGui::Separator();
				wstring textTrans = L"[" + mIter->second->Name() + L"]translate";
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
				ImGui::Text(String::ToString(textTrans).c_str());
				D3DXVECTOR3 position;
				mIter->second->Position(&position);
				ImGui::DragFloat3("##T", (float*)&position, 0.05f, -500, 500);
				mIter->second->Position(position);

				wstring textScale = L"[" + mIter->second->Name() + L"]scale";
				ImGui::Text(String::ToString(textScale).c_str());
				D3DXVECTOR3 scale;
				mIter->second->Scale(&scale);
				ImGui::DragFloat3("##S", (float*)&scale, 0.05f, -100, 100);
				mIter->second->Scale(scale);


				wstring textRotate = L"[" + mIter->second->Name() + L"]rotate";
				ImGui::Text(String::ToString(textRotate).c_str());
				D3DXVECTOR3 rotate;
				mIter->second->Rotation(&rotate);
				ImGui::DragFloat3("##R", (float*)&rotate, 0.05f, -3.14f, 3.14f);
				mIter->second->Rotation(rotate);

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
				if (ImGui::TreeNode("Diffuse"))
				{
					ImGui::Text("Model Diffuse");

					D3DXCOLOR diffuse = mIter->second->GetMaterial()->GetDiffuse();

					ImGui::ColorEdit4("##d", diffuse);
					{
						mIter->second->GetMaterial()->SetDiffuse(diffuse);
					}

					ImGui::Text("Model DiffuseMap");
					if (mIter->second->GetMaterial()->GetDiffuseMap() != NULL)
					{
						if (ImGui::ImageButton(mIter->second->GetMaterial()->GetDiffuseMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 0));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 0));
						}
					}
					ImGui::TreePop();
				}

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
				if (ImGui::TreeNode("Specular"))
				{
					ImGui::Text("Model Specular");

					D3DXCOLOR specular = mIter->second->GetMaterial()->GetSpecular();
					ImGui::ColorEdit4("##s", specular);
					{
						mIter->second->GetMaterial()->SetSpecular(specular);
					}
					ImGui::Text("Shininess");
					ImGui::DragFloat("##shiness", &specular.a, 1, 1, 50);
					{
						mIter->second->GetMaterial()->SetSpecular(specular);
					}

					ImGui::Text("Model SpecularMap");
					if (mIter->second->GetMaterial()->GetSpecularMap() != NULL)
					{
						if (ImGui::ImageButton(mIter->second->GetMaterial()->GetSpecularMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 1));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 1));
						}
					}
					ImGui::TreePop();
				}

				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
				if (ImGui::TreeNode("NormalMap"))
				{
					ImGui::Text("Model NormalMap");
					if (mIter->second->GetMaterial()->GetNormalMap() != NULL)
					{
						if (ImGui::ImageButton(mIter->second->GetMaterial()->GetNormalMap()->SRV(), ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 2));
						}
					}
					else
					{
						if (ImGui::ImageButton(0, ImVec2(100, 100)))
						{
							Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadMeshMapFile, this, mIter->second, placeholders::_1, 2));
						}
					}
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");
				if (ImGui::BeginCombo("Collider Type", String::ToString(selectColliderName).c_str()))
				{
					for (size_t i = 0; i < colliderNames.size(); i++)
					{
						bool isSelected = (selectColliderName == colliderNames[i]);
						if (ImGui::Selectable(String::ToString(colliderNames[i]).c_str(), isSelected))
						{
							selectColliderName = colliderNames[i];
						}
						if (isSelected == true)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("CreateCollider"))
				{
					if (selectColliderName == L" Box")
						mIter->second->CreateCollider(0);
					else if (selectColliderName == L" Line")
						mIter->second->CreateCollider(1);
					else if (selectColliderName == L" Sphere")
						mIter->second->CreateCollider(2);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "Colliders[%d]", mIter->second->GetColliders().size());
				ImGui::SameLine();
				if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
				{
					if (!eType.isColliderOpen)eType.isColliderOpen = true;
					else eType.isColliderOpen = false;
				}

				if (ImGui::Button("Delete Back") && mIter->second->GetColliders().size()>0)
				{
					vector<ColliderElement*>::iterator cIter;

					cIter = mIter->second->GetColliders().begin();


					while (true)
					{
						if (cIter == mIter->second->GetColliders().end())
						{
							cIter--;
							break;
						}
						cIter++;
					}
					SAFE_DELETE(*cIter);
					mIter->second->GetColliders().pop_back();
				}

				if (eType.isColliderOpen)
				{
					for (size_t i = 0; i < mIter->second->GetColliders().size(); i++)
					{
						if (ImGui::TreeNode((String::ToString(mIter->second->GetColliders()[i]->Name())).c_str()))
						{
							mIter->second->GetColliders()[i]->ImguiRender();

							ImGui::TreePop();
						}
						else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
						{
							ImGui::SetDragDropPayload("Collider", &mIter->second->GetColliders()[i], sizeof(ColliderElement*));
							ImGui::Text(String::ToString(mIter->second->GetColliders()[i]->Name()).c_str());
							ImGui::EndDragDropSource();
						}
					}
				}

				ImGui::Separator();
				ImGui::Separator();
			}
		}
		//==================================================================================//


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
								if (ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetDiffuseMap()->SRV(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 0));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 0));
								}
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
								if (ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetSpecularMap()->SRV(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 1));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 1));
								}
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
									if (ImGui::ImageButton(modelIter->second->GetModel()->Materials()[i]->GetNormalMap()->SRV(), ImVec2(100, 100)))
									{
										Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 2));
									}
								}
								else
								{
									if (ImGui::ImageButton(0, ImVec2(100, 100)))
									{
										Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, modelIter->second->GetModel()->Materials()[i], placeholders::_1, 2));
									}
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
				if (ImGui::BeginCombo("Collider Type", String::ToString(selectColliderName).c_str()))
				{
					for (size_t i = 0; i < colliderNames.size(); i++)
					{
						bool isSelected = (selectColliderName == colliderNames[i]);
						if (ImGui::Selectable(String::ToString(colliderNames[i]).c_str(), isSelected))
						{
							selectColliderName = colliderNames[i];
						}
						if (isSelected == true)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("CreateCollider"))
				{
					if (selectColliderName == L" Box")
						modelIter->second->CreateCollider(0);
					else if (selectColliderName == L" Line")
						modelIter->second->CreateCollider(1);
					else if (selectColliderName == L" Sphere")
						modelIter->second->CreateCollider(2);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "Colliders[%d]", modelIter->second->GetColliders().size());
				ImGui::SameLine();
				if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
				{
					if (!eType.isColliderOpen)eType.isColliderOpen = true;
					else eType.isColliderOpen = false;
				}

				if (ImGui::Button("Delete Back") && modelIter->second->GetColliders().size()>0)
				{
					vector<ColliderElement*>::iterator cIter;

					cIter = modelIter->second->GetColliders().begin();


					while (true)
					{
						if (cIter == modelIter->second->GetColliders().end())
						{
							cIter--;
							break;
						}
						cIter++;
					}
					SAFE_DELETE(*cIter);
					modelIter->second->GetColliders().pop_back();
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

				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Trails]");
				if (ImGui::Button("CreateTrails"))
				{
					static UINT trailNumber = 0;
					TrailRenderer*trail = new TrailRenderer(modelIter->second, trailEffect);

					trail->Name() = modelIter->second->Name() + L"Trail" + to_wstring(trailNumber);
					trail->Ready();
					trail->SetRun(true);

					modelIter->second->GetTrails().push_back(trail);
					trailNumber++;
				}
				ImGui::TextColored(ImVec4(1, 1, 1, 1), "Trail Count[%d]", modelIter->second->GetTrails().size());
				ImGui::SameLine();
				if (ImGui::ArrowButton("Trails", ImGuiDir_Down))
				{
					if (!eType.isTrailOpen)eType.isTrailOpen = true;
					else eType.isTrailOpen = false;
				}

				if (eType.isTrailOpen)
				{
					for (size_t i = 0; i < modelIter->second->GetTrails().size(); i++)
					{
						if (ImGui::TreeNode((String::ToString(modelIter->second->GetTrails()[i]->Name())).c_str()))
						{
							modelIter->second->GetTrails()[i]->ImGuiRender();

							ImGui::TreePop();
						}
						else if (ImGui::BeginDragDropTarget())//여기서뼈받아오면 그뼈의 인덱스번호넘긴다
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Bone"))
							{
								IM_ASSERT(payload->DataSize == sizeof(ModelBone));
								ModelBone*data = (ModelBone*)payload->Data;

								modelIter->second->GetTrails()[i]->SetBone(data->Index());

							}
							ImGui::EndDragDropTarget();
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
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Setting Type]");
				ImGui::RadioButton("Model Set", (int*)&AnimType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Animation Set", (int*)&AnimType, 1);
				ImGui::Separator();

				CharactorSetRender(aIter->second);
			}
		}
		//==================================================================================//
		ImGui::End();
	}
}

void ObjectTool::DrawHiarachy()
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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("meshLoad"))
			{
				/*IM_ASSERT(payload->DataSize == sizeof(Mesh*));
				Mesh** data = (Mesh**)payload->Data;

				Mesh*copy = new Mesh(**data);
				copy->Initialize();
				copy->Ready();
				copy->Name() = (*data)->Name();

				terrains[(*data)->Name()] = copy;
				shadow->Add(terrains[(*data)->Name()]);
				shadow->Add(terrains[(*data)->Name()]->GetBillboard());*/
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
			meshIter mIter = meshes.find(selectObjName);
			if (mIter != meshes.end())
			{
				shadow->Erase(mIter->second);
				gizmo->Erase(mIter->second);
				ColliderManager::Get()->Erase(mIter->second);
				SAFE_DELETE(mIter->second);
				meshes.erase(mIter);
			}

			modelIter modelIter = models.find(selectObjName);
			if (modelIter != models.end())
			{
				//shadow->Erase(modelIter->second);
				gizmo->Erase(modelIter->second);
				ColliderManager::Get()->Erase(modelIter->second);
				InstanceManager::Get()->DeleteModel(modelIter->second,modelIter->second->InstanceID(),InstanceManager::InstanceType::MODEL);
				//SAFE_DELETE(modelIter->second);
				models.erase(modelIter);
			}

			animIter aIter = animModels.find(selectObjName);
			if (aIter != animModels.end())
			{
				map<wstring, vector<BoneSphere*>>::iterator boneIter;
				boneIter = boneMeshes.find(selectObjName);

				for (size_t i = 0; i<boneIter->second.size(); i++)
				{
					SAFE_DELETE(boneIter->second[i]);
				}
				boneIter->second.clear();
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

		//======================================메쉬=====================================//
		if (ImGui::SmallButton("Meshes"))
		{
			if (!eType.isShowCombCharactor)eType.isShowCombCharactor = true;
			else eType.isShowCombCharactor = false;
		}

		ImGui::Separator();
		if (eType.isShowCombCharactor)
		{
			meshIter mIter = meshes.begin();
			for (; mIter != meshes.end(); mIter++)
			{
				if (ImGui::TreeNode(String::ToString(mIter->second->Name()).c_str()))
				{
					PickedInit();
					mIter->second->SetPickState(true);
					if (selectObjName != mIter->first)
						selectObjName = mIter->first;

					ImGui::TreePop();
				}
				else
				{
					mIter->second->SetPickState(false);

					//누군가선점하고있을떄는 이렇게바꿀수없음
					selectObjName = L"";
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					ImGui::SetDragDropPayload("Mesh", mIter->second, sizeof(Mesh*));
					ImGui::Text(String::ToString(mIter->second->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
		}
		//==================================================================================//

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
			modelIter modelIter = models.begin();
			for (; modelIter != models.end(); modelIter++)
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
					selectObjName = L"";
				}

				if (ImGui::BeginDragDropSource())
				{
					if (modelIter->second->TypeName() == L"Model")
					{
						ImGui::SetDragDropPayload("Model", modelIter->second, sizeof(GameModel*));
						ImGui::Text(String::ToString(modelIter->second->Name()).c_str());
						ImGui::EndDragDropSource();
					}
					else if (modelIter->second->TypeName() == L"Weapon")
					{
						ImGui::SetDragDropPayload("Weapon", modelIter->second, sizeof(Weapon*));
						ImGui::Text(String::ToString(modelIter->second->Name()).c_str());
						ImGui::EndDragDropSource();
					}
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
			animIter aIter = animModels.begin();
			for (; aIter != animModels.end(); aIter++)
			{
				if (ImGui::TreeNode(String::ToString(aIter->second->Name()).c_str()))
				{
					PickedInit();
					aIter->second->SetPickState(true);
					if (selectObjName != aIter->first)
						selectObjName = aIter->first;

					//뼈가 2개이상 없는모델은 뺀다
					if (aIter->second->GetModel()->BoneCount() < 2)
						ImGui::TreePop();
					else
					{
						ShowBoneNode(aIter->second, aIter->second->GetModel()->BoneByIndex(0)->Index());
						ImGui::TreePop();
					}
				}
				else
				{
					aIter->second->SetPickState(false);
					selectObjName = L"";
				}

				if (ImGui::BeginDragDropSource())//터레인모델넘길때
				{
					if (aIter->second->TypeName() == L"Animation")
					{
						ImGui::SetDragDropPayload("Animation", aIter->second, sizeof(GameAnimator*));
						ImGui::Text(String::ToString(aIter->second->Name()).c_str());
						ImGui::EndDragDropSource();
					}
					else if (aIter->second->TypeName() == L"Player")
					{
						ImGui::SetDragDropPayload("Player", aIter->second, sizeof(Player*));
						ImGui::Text(String::ToString(aIter->second->Name()).c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}
		//==================================================================================//
		ImGui::Separator();
	}
	ImGui::End();
}

void ObjectTool::CheckSelectObject()
{
	PickedInit();

	meshIter mIter = meshes.find(selectObjName);
	if (mIter != meshes.end())
	{
		mIter->second->SetPickState(true);
		return;
	}

	modelIter modelIter = models.find(selectObjName);
	if (modelIter != models.end())
	{
		modelIter->second->SetPickState(true);
		return;
	}
}
void ObjectTool::LoadMeshMapFile(Mesh * mesh, wstring file, int type)
{
	switch (type)
	{
		case 0:
		{
			mesh->GetMaterial()->SetDiffuseMap(file);
		}
		break;
		case 1:
		{
			mesh->GetMaterial()->SetSpecularMap(file);
		}
		break;
		case 2:
		{
			mesh->GetMaterial()->SetNormalMap(file);
		}
		break;
	}
}

void ObjectTool::LoadModelMapFile(Material*material, wstring file, int type)
{
	switch (type)
	{
		case 0:
		{
			material->SetDiffuseMap(file);
		}
		break;
		case 1:
		{
			material->SetSpecularMap(file);
		}
		break;
		case 2:
		{
			material->SetNormalMap(file);
		}
		break;
	}
}

void ObjectTool::ExportFile(wstring fileName)
{
	FbxLoader*loader = NULL;
	if (eType.isLoadMesh == true)
	{
		wstring saveFolderName = String::ToWString(savePath) + L"/";
		loader = new FbxLoader
		(
			fileName,
			Models + saveFolderName,
			String::ToWString(savePath)
		);

		loader->ExportMaterial();
		loader->ExportMesh();

		SAFE_DELETE(loader);
	}
	if (eType.isLoadMesh == false)
	{
		wstring saveFolderName = String::ToWString(savePath) + L"/";
		loader = new FbxLoader
		(
			fileName,
			Models + saveFolderName,
			Path::GetFileName(fileName)
		);

		loader->ExportAnimation(0);
		SAFE_DELETE(loader);
	}
}

void ObjectTool::LoadModel(wstring fileName)
{
	string file = String::ToString(fileName);

	int dotIndex = file.find(".");
	int nameSize = file.size();

	file.erase(dotIndex, nameSize);

	static UINT instModelId = 0;
	GameModel*model = new GameModel
	(
		instModelId,
		String::ToWString(file) + L".material",
		String::ToWString(file) + L".mesh",
		lineEffect
	);

	model->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(models.size());
	model->Ready();
	model->TypeName() = L"Model";
	models[model->Name()] = model;

	gizmo->SetSelectionPool(models[model->Name()]);
	ColliderManager::Get()->InputObject(models[model->Name()]);
	FileManager::Get()->DataPush(FileManager::FileType::Model, models[model->Name()]);
}

void ObjectTool::LoadWeapon(Player*player, wstring fileName)
{
	string file = String::ToString(fileName);

	int dotIndex = file.find(".");
	int nameSize = file.size();

	file.erase(dotIndex, nameSize);

	static UINT instWeaponId = 0;
	Weapon*weapon = new Weapon
	(
		instWeaponId,
		String::ToWString(file) + L".material",
		String::ToWString(file) + L".mesh",
		player,
		lineEffect
	);

	weapon->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(instWeaponId);
	weapon->Ready();
	weapon->TypeName() = L"Weapon";
	player->GetWeapons().push_back(weapon);

	instWeaponId++;

	gizmo->SetSelectionPool(player->GetWeapons()[player->GetWeapons().size() - 1]);
	ColliderManager::Get()->InputObject(player->GetWeapons()[player->GetWeapons().size() - 1]);
}

void ObjectTool::LoadCharactor(wstring fileName, MENUTYPE type)
{
	string file = String::ToString(fileName);

	int dotIndex = file.find(".");
	int nameSize = file.size();

	file.erase(dotIndex, nameSize);

	switch (type)
	{
		case GameTool::LOAD_ANIMATION:
		{
			static UINT instAnimId = 0;
			GameAnimator*model = new GameAnimator
			(
				instAnimId,
				String::ToWString(file) + L".material",
				String::ToWString(file) + L".mesh",
				lineEffect
			);

			model->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(models.size());
			model->Ready();
			model->TypeName() = L"Animation";

			animModels[model->Name()] = model;

			CreateBone(animModels[model->Name()]);

			gizmo->SetSelectionPool(animModels[model->Name()]);
			ColliderManager::Get()->InputObject(animModels[model->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Animation, animModels[model->Name()]);
		}
		break;
		case GameTool::LOAD_PLAYER:
		{
			static UINT instPlayerId = 0;
			Player*model = new Player
			(
				instPlayerId,
				String::ToWString(file) + L".material",
				String::ToWString(file) + L".mesh",
				lineEffect
			);

			model->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(models.size());
			model->Ready();
			model->TypeName() = L"Player";

			animModels[model->Name()] = model;

			CreateBone(animModels[model->Name()]);

			gizmo->SetSelectionPool(animModels[model->Name()]);
			ColliderManager::Get()->InputObject(animModels[model->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Player, animModels[model->Name()]);
		}
		break;
		case GameTool::LOAD_ENEMY1:
		{
			static UINT instEnemy1Id = 0;
			Enemy*model = new NearEnemy
			(
				instEnemy1Id,
				String::ToWString(file) + L".material",
				String::ToWString(file) + L".mesh",
				lineEffect
			);

			model->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(models.size());
			model->Ready();
			model->TypeName() = L"NearEnemy";

			animModels[model->Name()] = model;

			CreateBone(animModels[model->Name()]);

			gizmo->SetSelectionPool(animModels[model->Name()]);
			ColliderManager::Get()->InputObject(animModels[model->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[model->Name()]);
		}
		break;
		case GameTool::LOAD_BOSS:
		{
			static UINT instEnemy1Id = 0;
			Enemy*model = new Boss
			(
				instEnemy1Id,
				String::ToWString(file) + L".material",
				String::ToWString(file) + L".mesh",
				lineEffect
			);

			model->Name() = Path::GetFileName(String::ToWString(file)) + to_wstring(models.size());
			model->Ready();
			model->TypeName() = L"Boss";

			animModels[model->Name()] = model;

			CreateBone(animModels[model->Name()]);

			gizmo->SetSelectionPool(animModels[model->Name()]);
			ColliderManager::Get()->InputObject(animModels[model->Name()]);
			FileManager::Get()->DataPush(FileManager::FileType::Enemy1, animModels[model->Name()]);
		}
		break;
	}
}

void ObjectTool::LoadAnimation(GameAnimator*model, wstring fileName)
{
	model->AddClip(fileName, true);
}

void ObjectTool::CharactorSetRender(GameAnimator*anim)
{
	//ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
	//ImGui::Text("Center(%.2f,%.2f,%.2f)", sIter->second->GetCenterPos().x, miter->second->GetCenterPos().y, miter->second->GetCenterPos().z);

	//ImGui::Separator();
	//ImGui::Separator();

	switch (AnimType)
	{
		case ObjectTool::Model:
		{
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
							if (ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetDiffuseMap()->SRV(), ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 0));
							}
						}
						else
						{
							if (ImGui::ImageButton(0, ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 0));
							}
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
							if (ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetSpecularMap()->SRV(), ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 1));
							}
						}
						else
						{
							if (ImGui::ImageButton(0, ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 1));
							}
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
							if (ImGui::ImageButton(anim->GetModel()->Materials()[i]->GetNormalMap()->SRV(), ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 2));
							}
						}
						else
						{
							if (ImGui::ImageButton(0, ImVec2(100, 100)))
							{
								Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, anim->GetModel()->Materials()[i], placeholders::_1, 2));
							}
						}
						ImGui::TreePop();

					}
					ImGui::TreePop();
				}
			}



			ImGui::Separator();
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");
			if (ImGui::BeginCombo("Type", String::ToString(selectColliderName).c_str()))
			{
				for (size_t i = 0; i < colliderNames.size(); i++)
				{
					bool isSelected = (selectColliderName == colliderNames[i]);
					if (ImGui::Selectable(String::ToString(colliderNames[i]).c_str(), isSelected))
					{
						selectColliderName = colliderNames[i];
					}
					if (isSelected == true)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("CreateCollider"))
			{
				if (selectColliderName == L" Box")
					anim->CreateCollider(0);
				else if (selectColliderName == L" Line")
					anim->CreateCollider(1);
				else if (selectColliderName == L" Sphere")
					anim->CreateCollider(2);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "Collider Count[%d]", anim->GetColliders().size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
			{
				if (!eType.isColliderOpen)eType.isColliderOpen = true;
				else eType.isColliderOpen = false;
			}

			if (ImGui::Button("Delete Back") && anim->GetColliders().size()>0)
			{
				vector<ColliderElement*>::iterator cIter;

				cIter = anim->GetColliders().begin();


				while (true)
				{
					if (cIter == anim->GetColliders().end())
					{
						cIter--;
						break;
					}
					cIter++;
				}
				SAFE_DELETE(*cIter);
				anim->GetColliders().pop_back();
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


			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Trails]");
			if (ImGui::Button("CreateTrails"))
			{
				static UINT trailNumber = 0;
				TrailRenderer*trail = new TrailRenderer(anim, trailEffect);
				
				trail->Name() = anim->Name() + L"Trail" + to_wstring(trailNumber);
				trail->Ready();
				trail->SetRun(true);

				anim->GetTrails().push_back(trail);
				trailNumber++;
			}
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "Trail Count[%d]", anim->GetTrails().size());
			ImGui::SameLine();
			if (ImGui::ArrowButton("Trails", ImGuiDir_Down))
			{
				if (!eType.isTrailOpen)eType.isTrailOpen = true;
				else eType.isTrailOpen = false;
			}

			if (eType.isTrailOpen)
			{
				for (size_t i = 0; i < anim->GetTrails().size(); i++)
				{
					if (ImGui::TreeNode((String::ToString(anim->GetTrails()[i]->Name())).c_str()))
					{
						anim->GetTrails()[i]->ImGuiRender();

						ImGui::TreePop();
					}
					else if (ImGui::BeginDragDropTarget())//여기서뼈받아오면 그뼈의 인덱스번호넘긴다
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Bone"))
						{
							IM_ASSERT(payload->DataSize == sizeof(ModelBone));
							ModelBone*data = (ModelBone*)payload->Data;

							anim->GetTrails()[i]->SetBone(data->Index());

						}
						ImGui::EndDragDropTarget();
					}
				}
			}

			static bool isWeaponOpen = false;
			if (anim->TypeName() == L"Player")
			{
				ImGui::Separator();
				ImGui::Separator();

				Player*player = dynamic_cast<Player*>(anim);
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Weapons]");
				if (ImGui::Button("CreateWeapon"))
				{
					Path::OpenFileDialog(L"파일", L"", Models, bind(&ObjectTool::LoadWeapon, this, player, placeholders::_1));
				}
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
											if (ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetDiffuseMap()->SRV(), ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 0));
											}
										}
										else
										{
											if (ImGui::ImageButton(0, ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 0));
											}
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
											if (ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetSpecularMap()->SRV(), ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 1));
											}
										}
										else
										{
											if (ImGui::ImageButton(0, ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 1));
											}
										}
										ImGui::TreePop();
									}
									ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
									if (ImGui::TreeNode("NormalMap"))
									{
										ImGui::Text("Model NormalMap");

										if (player->GetWeapons()[i]->GetModel()->Materials()[j]->GetNormalMap() != NULL)
										{
											if (ImGui::ImageButton(player->GetWeapons()[i]->GetModel()->Materials()[j]->GetNormalMap()->SRV(), ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 2));
											}
										}
										else
										{
											if (ImGui::ImageButton(0, ImVec2(100, 100)))
											{
												Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ObjectTool::LoadModelMapFile, this, player->GetWeapons()[i]->GetModel()->Materials()[j], placeholders::_1, 2));
											}
										}
										ImGui::TreePop();
									}
									ImGui::TreePop();
								}
							}
							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");
							if (ImGui::BeginCombo("Type", String::ToString(selectColliderName).c_str()))
							{
								for (size_t i = 0; i < colliderNames.size(); i++)
								{
									bool isSelected = (selectColliderName == colliderNames[i]);
									if (ImGui::Selectable(String::ToString(colliderNames[i]).c_str(), isSelected))
									{
										selectColliderName = colliderNames[i];
									}
									if (isSelected == true)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}

							if (ImGui::Button("CreateCollider"))
							{
								if (selectColliderName == L" Box")
									player->GetWeapons()[i]->CreateCollider(0);
								else if (selectColliderName == L" Line")
									player->GetWeapons()[i]->CreateCollider(1);
								else if (selectColliderName == L" Sphere")
									player->GetWeapons()[i]->CreateCollider(2);
							}

							static bool isWeaponCollider = false;
							ImGui::TextColored(ImVec4(1, 1, 1, 1), "Collider Count[%d]", player->GetWeapons()[i]->GetColliders().size());
							ImGui::SameLine();
							if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
							{
								if (!isWeaponCollider)isWeaponCollider = true;
								else isWeaponCollider = false;
							}

							if (ImGui::Button("Delete Back") && player->GetWeapons()[i]->GetColliders().size()>0)
							{
								vector<ColliderElement*>::iterator cIter;

								cIter = player->GetWeapons()[i]->GetColliders().begin();


								while (true)
								{
									if (cIter == player->GetWeapons()[i]->GetColliders().end())
									{
										cIter--;
										break;
									}
									cIter++;
								}
								SAFE_DELETE(*cIter);
								player->GetWeapons()[i]->GetColliders().pop_back();
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
									else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
									{
										ImGui::SetDragDropPayload("Collider", &player->GetWeapons()[i]->GetColliders()[j], sizeof(ColliderElement*));
										ImGui::Text(String::ToString(player->GetWeapons()[i]->GetColliders()[j]->Name()).c_str());
										ImGui::EndDragDropSource();
									}
								}
							}

							ImGui::TreePop();
						}
						else if (ImGui::BeginDragDropTarget())//여기서뼈받아오면 그뼈의 인덱스번호넘긴다
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Bone"))
							{
								IM_ASSERT(payload->DataSize == sizeof(ModelBone));
								ModelBone*data = (ModelBone*)payload->Data;

								player->GetWeapons()[i]->TargetWorld(data->Index());

							}
							ImGui::EndDragDropTarget();
						}
					}//Weapon 내 정보들
				}
			}
		}
		break;
		case ObjectTool::Animation:
		{
			UpdateBone(anim);

			if (anim->TypeName() == L"Player"|| anim->TypeName() == L"NearEnemy")
			{
				if (anim->TypeName() == L"Player")
				{
					Player*player = dynamic_cast<Player*>(anim);

					static bool playerBehavior = false;
					if (ImGui::Button("Open Behavior Tree"))
					{
						if (!playerBehavior)playerBehavior = true;
						else playerBehavior = false;
					}
					if (playerBehavior)
					{
						player->GetBT()->ImguiRender();
					}
				}
				else if (anim->TypeName() == L"NearEnemy")
				{
					Enemy*enemy = dynamic_cast<NearEnemy*>(anim);

					static bool nearEnemyBehavior = false;
					if (ImGui::Button("Open Behavior Tree"))
					{
						if (!nearEnemyBehavior)nearEnemyBehavior = true;
						else nearEnemyBehavior = false;
					}
					if (nearEnemyBehavior)
					{
						enemy->GetBT()->ImguiRender();
					}
				}
			}

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Animation]");
			ImGui::Separator();
			ImGui::BulletText("Animation Count : %d", anim->AnimClips().size());

			if (ImGui::BeginCombo("##Animation", String::ToString(animName).c_str()))
			{
				map<wstring, ModelClip*>::iterator clipIter = anim->AnimClips().begin();
				for (; clipIter != anim->AnimClips().end(); clipIter++)
				{
					bool isSelected = (animName == clipIter->second->AnimName());
					if (ImGui::Selectable(String::ToString(clipIter->second->AnimName()).c_str(), isSelected))
					{
						animName = clipIter->second->AnimName();
						anim->StartTargetClip(animName);
					}
					if (isSelected == true)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Push Anim"))
			{
				Path::OpenFileDialog(L"파일", L".png", Models, bind(&ObjectTool::LoadAnimation, this, anim, placeholders::_1));
			}

			if (ImGui::Button("Play", ImVec2(65, 30)))anim->Play();
			ImGui::SameLine();
			if (ImGui::Button("Pause", ImVec2(65, 30)))anim->Pause();
			ImGui::SameLine();
			if (ImGui::Button("Stop", ImVec2(65, 30)))anim->Stop();

			map<wstring, ModelClip*>::iterator clipIter = anim->AnimClips().find(anim->CurClipName());

			if (clipIter != anim->AnimClips().end())
			{
				ImGui::Text("[BlendTime]");
				ImGui::SliderFloat("##blend", &clipIter->second->BlendTime(), 0, 50);
				ImGui::Text("[StartFrame]");
				ImGui::SliderInt("##startframe", &clipIter->second->StartFrame(), 0, clipIter->second->FrameCount() - 2);
				ImGui::Text("[EndFrame]");
				ImGui::SliderInt("##endframe", &clipIter->second->EndFrame(), 0, clipIter->second->FrameCount() - 2);
				ImGui::Text("[FrameRate]");
				ImGui::SliderInt("##frame", (int*)&anim->CurrentKeyFrame(), 0, clipIter->second->FrameCount() - 2);
				ImGui::Text("[Animation Speed]");
				ImGui::DragFloat("##speed", &clipIter->second->Speed(), 0.02f, 0, 10);
				ImGui::Text("[Use Repeat]");
				ImGui::SameLine();
				ImGui::Checkbox("##repeat", &clipIter->second->IsRepeat());
				ImGui::Text("[Use InPlace]");
				ImGui::SameLine();
				ImGui::Checkbox("##inplace", &clipIter->second->IsInPlace());
			}

			RenderBone();
		}
		break;
	}
	ImGui::Separator();
	ImGui::Separator();
}

void ObjectTool::CreateBone(GameAnimator*model)
{
	for (UINT i = 0; i < model->GetModel()->BoneCount(); i++)
	{
		ModelBone*bone = model->GetModel()->BoneByIndex(i);
		BoneSphere*boneSphere = new BoneSphere(meshMaterial);

		boneSphere->Name() = bone->Name();

		//구의 위치를 본의 위치로 바꿔야한다.
		//본의 메트릭스에서 위치값만 빼온다.

		D3DXMATRIX boneMatrix = model->GetBoneMatrix(i); // 이미월드로 변환된뼈행렬

		boneSphere->InitScale() = { 0.015f, 0.015f, 0.015f };
		boneSphere->InitPos() = { boneMatrix._41, boneMatrix._42, boneMatrix._43 };

		boneMeshes[model->Name()].push_back(boneSphere);
	}
}

void ObjectTool::ShowBoneNode(GameAnimator * model, int index)
{
	if (model->GetModel()->BoneByIndex(index) == NULL)return;

	if (index == 0)
	{
		if (model->GetModel()->BoneByIndex(index)->ChildCount() == 0)
			index = 1;
		else
			index = 0;
	}

	wstring boneName = model->GetModel()->BoneByIndex(index)->Name();
	if (ImGui::TreeNode((String::ToString(boneName) + "[" + to_string(index) + "]").c_str()))
	{
		for (size_t i = 0; i < model->GetModel()->BoneByIndex(index)->GetChilds().size(); i++)
		{
			ShowBoneNode(model, model->GetModel()->BoneByIndex(index)->child(i)->Index());
		}

		ImGui::TreePop();
	}
	else if (ImGui::BeginDragDropSource())//터레인모델넘길때
	{
		ImGui::SetDragDropPayload("Bone", model->GetModel()->BoneByIndex(index), sizeof(ModelBone));
		ImGui::Text(String::ToString(boneName).c_str());
		ImGui::EndDragDropSource();
	}

	//else if (ImGui::BeginDragDropTarget())//머터리얼 받을 타겟
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon"))
	//	{
	//		IM_ASSERT(payload->DataSize == sizeof(Weapon*));
	//		Weapon**data = (Weapon**)payload->Data;

	//		(*data)->TargetWorld(model->GetBoneWorld(index));
	//	}
	//	ImGui::EndDragDropTarget();
	//}

}

void ObjectTool::PostRenderBoneName()
{
	map<wstring, vector<BoneSphere*>>::iterator biter = boneMeshes.begin();

	for (; biter != boneMeshes.end(); biter++)
	{
		for (size_t i = 0; i < biter->second.size(); i++)
			biter->second[i]->PostBoneRender();
	}
}

void ObjectTool::LoadModelData(wstring fileName)
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


	static UINT instLoadId = 0;
	if (type == L"Player")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Player*player = new Player(
			instLoadId,
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
	else if (type == L"Model")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		GameModel*model = new GameModel(
			instLoadId,
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
			instLoadId,
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
	else if (type == L"NearEnemy")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Enemy*enemy = new NearEnemy(
			instLoadId,
			meshName,
			materialName,
			lineEffect);

		enemy->LoadStart() = true;
		enemy->Load(fileName);
		enemy->Ready();

		wstring enemyName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = enemyName.find(dot);
		int size = enemyName.size();

		enemyName.erase(index, size);

		enemy->Name() = enemyName;

		loadCharactor.push_back(enemy);
	}
	else if (type == L"Boss")
	{
		node = node->NextSiblingElement();
		wstring materialName = String::ToWString(node->GetText());

		node = node->NextSiblingElement();
		wstring meshName = String::ToWString(node->GetText());

		Enemy*enemy = new Boss(
			instLoadId,
			meshName,
			materialName,
			lineEffect);

		enemy->LoadStart() = true;
		enemy->Load(fileName);
		enemy->Ready();

		wstring enemyName = Path::GetFileName(fileName);

		wstring dot = L".";
		int index = enemyName.find(dot);
		int size = enemyName.size();

		enemyName.erase(index, size);

		enemy->Name() = enemyName;

		loadCharactor.push_back(enemy);
	}
}

void ObjectTool::RenderBone()
{
	map<wstring, vector<BoneSphere*>>::iterator biter = boneMeshes.begin();

	for (; biter != boneMeshes.end(); biter++)
	{
		for (size_t i = 0; i < biter->second.size(); i++)
		{
			biter->second[i]->BoneRender();

			if (biter->second[i]->IsPicked())
				biter->second[i]->BoneImGuiRender();
		}
	}
}

void ObjectTool::UpdateBone(GameAnimator*model)
{
	//피킹하면 색깔바꾸기
	map<wstring, vector<BoneSphere*>>::iterator boneIter = boneMeshes.begin();
	for (; boneIter != boneMeshes.end(); boneIter++)
	{
		for (size_t i = 0; i < boneIter->second.size(); i++)
		{
			D3DXVECTOR3 modelScale;
			model->Scale(&modelScale);

			D3DXVECTOR3 modelRotate;
			model->Rotation(&modelRotate);

			D3DXVECTOR3 modelTrans;
			model->Position(&modelTrans);

			D3DXVECTOR3 scale =
			{
				boneIter->second[i]->InitScale().x*modelScale.x,
				boneIter->second[i]->InitScale().y*modelScale.y,
				boneIter->second[i]->InitScale().z*modelScale.z
			};

			//모델 메트릭스
			D3DXMATRIX S, R, T;
			D3DXMatrixScaling(&S, modelScale.x, modelScale.y, modelScale.z);
			D3DXMatrixRotationYawPitchRoll(&R, modelRotate.y, modelRotate.x, modelRotate.z);
			D3DXMatrixTranslation(&T, modelTrans.x, modelTrans.y, modelTrans.z);

			model->SetBoneMatrix(boneIter->second[i]->GetSetMatrix(), i);

			D3DXMATRIX boneMatrix = model->GetBoneMatrix(i)*S*R*T; // 이미월드로 변환된뼈행렬

			boneIter->second[i]->Scale(scale);
			boneIter->second[i]->Position(boneMatrix._41, boneMatrix._42, boneMatrix._43);


			boneIter->second[i]->PickUpdate();
		}
	}
}




