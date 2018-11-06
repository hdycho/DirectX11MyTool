#include "stdafx.h"
#include "ToolManager.h"
#include "../Objects/GameModel.h"
#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Model/ModelBone.h"
#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshPlane.h"
#include "../Physics/ColliderBox.h"
#include "../LandScape/TerrainRender.h"
#include "../LandScape/Water.h"
#include "../Fbx/Exporter.h"
#include "../Utilities/Xml.h"
#include "../Viewer/Frustum.h"
#include "../Light/Shadow.h"
#include "../Objects/WeaponModel.h"
#include "../Systems/Window.h"

ToolManager::ToolManager(ExecuteValues*values)
	:values(values)
{
	//물체타입 Charactor,Object

	tools[TOOLTYPE::OBJECTTOOL] = new CharactorTool(values);
	tools[TOOLTYPE::TERRAINTOOL] = new MapTool(values);
	tools[TOOLTYPE::CAMERATOOL] = new CameraTool(values);
	curToolType = TOOLTYPE::NONE;
	sType = SCENETYPE::EDITSCENE;

	isOpen = true; // 처음에는 모든창열리게

	selectTagName = L" DefaultAnim";
	tagNames.push_back(L" DefaultAnim");
	tagNames.push_back(L" DefaultModel");
	tagNames.push_back(L" Player");
	tagNames.push_back(L" Weapon");

	CharactorShader= new Shader(Shaders + L"033_Animation.hlsl");

	Perspective*perspective = new Perspective(values->Viewport->GetWidth(), values->Viewport->GetHeight()
		, Math::PI*0.25f, 1, 1000);

	
	//현재 라이트는 게임모델밖에 적용안됨
	LIGHT->Init(values);
	CAM->Init(values);
	frustum = new Frustum(values, 900, values->MainCamera, perspective);
	CAM->InputCameraPtr(frustum);
}

ToolManager::~ToolManager()
{
}

void ToolManager::Render()
{
	//================================기본창================================//
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Export Fbx"))
				{
					mType = EXPORT_FBX;
				}
				if (ImGui::MenuItem("Load Data"))
				{
					mType = LOAD_MODEL;
				}
				if (ImGui::MenuItem("Save Data"))
				{
					mType = SAVE_MODEL;
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
					curToolType = TOOLTYPE::NONE;
					sType = SCENETYPE::GAMESCENE;
					PickSetting(false);
					PickedInit();
					isOpen = false;
				}
				if (ImGui::MenuItem("EditScene"))
				{
					curToolType = TOOLTYPE::NONE;
					sType = SCENETYPE::EDITSCENE;
					PickSetting(true);
					isOpen = true;
					objIter oiter = objects.begin();
					for (; oiter != objects.end(); oiter++)
					{
						modelIter miter = oiter->second.begin();

						for (; miter != oiter->second.end(); miter++)
						{
							//TODO:초기화코드인데 구현해야함
							miter->second->FixedInit(); 
						}
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Light"))
				{

				}
				if (ImGui::MenuItem("ObjectTool"))
				{
					curToolType = TOOLTYPE::OBJECTTOOL;
					PickSetting(false);

					objIter oiter = objects.begin();
					for (; oiter != objects.end(); oiter++)
					{
						modelIter miter = oiter->second.find(selectmodelName);
						if (miter == oiter->second.end())continue;
						tools[curToolType]->InputModelPtr(miter->second);
						tools[curToolType]->SetAnimVec(animVec);
						tools[curToolType]->SetType(0);
					}
				}
				if (ImGui::MenuItem("TerrainTool"))
				{
					curToolType = TOOLTYPE::TERRAINTOOL;
					PickSetting(false);
					tools[curToolType]->InputTerrainPtr(terrains[selectmodelName]);
					tools[curToolType]->SetType(0);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create Objects"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					static int cubeNum = 0;
					objects[L"Object"][L"Cube"+to_wstring(cubeNum)] = new MeshCube(values);
					Shader*shader = new Shader(Shaders + L"018_NormalMap.hlsl");
					objects[L"Object"][L"Cube"+to_wstring(cubeNum)]->ModelName() = L"Cube" + to_wstring(cubeNum);
					objects[L"Object"][L"Cube" + to_wstring(cubeNum)]->SetShader(shader);

					MeshCube*cube = dynamic_cast<MeshCube*>(objects[L"Object"][L"Cube" + to_wstring(cubeNum)]);
					
					//TODO:추가코드임
					objIter oiter = objects.find(L"Charactor");
					modelIter miter;
					if (oiter != objects.end())
					{
						objIter oiter = objects.find(L"Charactor");
						modelIter miter = oiter->second.begin();

						for (; miter != oiter->second.end(); miter++)
						{
							Player*p = dynamic_cast<Player*>(miter->second);
							cube->InputPlayerPtr(p);
						}
					}

					LIGHT->GetShadow()->Add(objects[L"Object"][L"Cube" + to_wstring(cubeNum)]);
					modelNames.push_back(objects[L"Object"][L"Cube" + to_wstring(cubeNum)]->ModelName());
					cubeNum++;
				}
				if (ImGui::MenuItem("Sphere"))
				{
					static int sphereNum = 0;
					objects[L"Object"][L"Sphere" + to_wstring(sphereNum)] = new MeshSphere(values);
					Shader*shader = new Shader(Shaders + L"018_NormalMap.hlsl");
					objects[L"Object"][L"Sphere" + to_wstring(sphereNum)]->ModelName() = L"Sphere" + to_wstring(sphereNum);
					objects[L"Object"][L"Sphere" + to_wstring(sphereNum)]->SetShader(shader);
					LIGHT->GetShadow()->Add(objects[L"Object"][L"Sphere" + to_wstring(sphereNum)]);
					modelNames.push_back(objects[L"Object"][L"Sphere" + to_wstring(sphereNum)]->ModelName());
					sphereNum++;
				}
				if (ImGui::MenuItem("Plane"))
				{
					static int planeNum = 0;
					objects[L"Object"][L"Plane" + to_wstring(planeNum)] = new MeshPlane(values);
					Shader*shader = new Shader(Shaders + L"018_NormalMap.hlsl");
					objects[L"Object"][L"Plane" + to_wstring(planeNum)]->ModelName() = L"Plane"+to_wstring(planeNum);
					objects[L"Object"][L"Plane" + to_wstring(planeNum)]->SetShader(shader);
					LIGHT->GetShadow()->Add(objects[L"Object"][L"Plane" + to_wstring(planeNum)]);
					modelNames.push_back(objects[L"Object"][L"Plane" + to_wstring(planeNum)]->ModelName());	
					planeNum++;
				}
				if (ImGui::MenuItem("Terrain"))
				{
					static int terrainNum = 0;
					terrains[L"Terrain" + to_wstring(terrainNum)] = new TerrainRender(values);
					terrains[L"Terrain" + to_wstring(terrainNum)]->name = L"Terrain" + to_wstring(terrainNum);
					
					//TODO:추가코드임
					objIter oiter = objects.find(L"Charactor");
					modelIter miter;
					if (oiter != objects.end())
					{
						miter = oiter->second.begin();

						for (; miter != oiter->second.end(); miter++)
						{
							Player*p = dynamic_cast<Player*>(miter->second);
							p->SetTerrain(terrains[L"Terrain" + to_wstring(terrainNum)]);
						}
					}

					modelNames.push_back(L"Terrain" + to_wstring(terrainNum));
					terrainNum++;
				}
				if (ImGui::MenuItem("Water"))
				{
					static int waterNum = 0;
					waters[L"Water"+ to_wstring(waterNum)]= new Water(values, 64, 64, 4, terrains[selectmodelName]->HeightMap());
					waters[L"Water" + to_wstring(waterNum)]->name= L"Water" + to_wstring(waterNum);
					modelNames.push_back(L"Water" + to_wstring(waterNum));
					waterNum++;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("GameManager"))
			{
				if (ImGui::MenuItem("CollisionManager"))
				{
					if (!eType.isShowCollision)
						eType.isShowCollision = true;
					else
						eType.isShowCollision = false;
				}
				if (ImGui::MenuItem("LightManager"))
				{
					if (!eType.isShowShadow)
						eType.isShowShadow = true;
					else
						eType.isShowShadow = false;
				}
				if (ImGui::MenuItem("CameraManager"))
				{
					
					if (!eType.isShowCamera)
						eType.isShowCamera = true;
					else
						eType.isShowCamera = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	//=====================================================================//


	if (isOpen)
	{
		//인스펙터창
		DrawInspector();
		//에셋창
		DrawAsset();
		//하이라키 창
		DrawHiarachy();
	}


	//==============================트리 랜더=================================//
	switch (curToolType)
	{
		case ToolManager::NONE:
		{
			if (!LIGHT->UseShadow())
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();

					for (; miter != oiter->second.end(); miter++)
					{
						miter->second->SetShader(CharactorShader);
						miter->second->Render();
					}
				}
			}
			else
				LIGHT->Render();

			objIter oiter = objects.begin();
			for (; oiter != objects.end(); oiter++)
			{
				modelIter miter = oiter->second.begin();

				for (; miter != oiter->second.end(); miter++)
				{
					miter->second->SetShader(CharactorShader);
					miter->second->PostRender();
				}
			}

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Render();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Render();
		}
		break;
		case ToolManager::OBJECTTOOL:
		{
			if (!LIGHT->UseShadow())
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();
					for (; miter != oiter->second.end(); miter++)
					{
						if (tools[curToolType]->GetModelPointer() == miter->second)continue;
						miter->second->SetShader(CharactorShader);
						miter->second->Render();
					}
				}
				tools[curToolType]->GetModelPointer()->SetShader(CharactorShader);
				tools[curToolType]->Render();
			}
			else
				LIGHT->Render();

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Render();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Render();
		}
		break;
		case ToolManager::TERRAINTOOL:
		{
			if (!LIGHT->UseShadow())
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();

					for (; miter != oiter->second.end(); miter++)
					{
						miter->second->SetShader(CharactorShader);
						miter->second->Render();
					}
				}
			}
			else
				LIGHT->Render();

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
			{
				if (tools[curToolType]->GetTerrainPointer() == tIter->second)continue;
				tIter->second->Render();
			}
			tools[curToolType]->Render();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Render();
		}
		break;
		case ToolManager::CAMERATOOL:
		{
			if (!LIGHT->UseShadow())
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();

					for (; miter != oiter->second.end(); miter++)
					{
						miter->second->SetShader(CharactorShader);
						miter->second->Render();
					}
				}
			}
			else
				LIGHT->Render();

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Render();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Render();

			tools[curToolType]->Render();
		}
		break;
	}
	//======================================================================//


	//==============================메니저 랜더==============================//
	if(eType.isShowCollision)
		COLLISION->Render();
	if (eType.isShowShadow)
		LIGHT->ImGuiRender();
	if(eType.isShowCamera)
		CAM->ImGuiRender();
	//======================================================================//

	frustum->Render();
}


void ToolManager::Update()
{
	//=================================FBX추출=================================//
	switch (mType)
	{
		case ToolManager::EXPORT_FBX:
		{
			Path::OpenFileDialog(L"파일", L".png", Models, bind(&ToolManager::ExportFile, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case ToolManager::LOAD_MODEL:
		{
			Path::OpenFileDialog(L"파일", L".png", Models, bind(&ToolManager::LoadFile, this, placeholders::_1));
			mType = MENU_NONE;
		}
		break;
		case ToolManager::SAVE_MODEL:
		{

		}
		break;
	}
	//======================================================================//

	switch (curToolType)
	{
		case ToolManager::NONE:
		{
			if (!LIGHT->UseShadow())
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();

					for (; miter != oiter->second.end(); miter++)
					{
						if (sType == SCENETYPE::GAMESCENE)
						{
							miter->second->FixedUpdate();
							COLLISION->Update();
						}
						miter->second->Update();
					}
				}
			}
			else
			{
				objIter oiter = objects.begin();
				for (; oiter != objects.end(); oiter++)
				{
					modelIter miter = oiter->second.begin();

					for (; miter != oiter->second.end(); miter++)
					{
						if (sType == SCENETYPE::GAMESCENE)
							miter->second->FixedUpdate();
					}
				}
				LIGHT->Update();
				COLLISION->Update();
			}
				

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Update();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Update();

		}
		break;
		case ToolManager::OBJECTTOOL:
		{
			objIter oiter = objects.begin();
			for (; oiter != objects.end(); oiter++)
			{
				modelIter miter = oiter->second.begin();
				for (; miter != oiter->second.end(); miter++)
				{
					if (tools[curToolType]->GetModelPointer() == miter->second)continue;
					miter->second->Update();
				}
			}
			tools[curToolType]->Update();

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Update();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Update();
		}
		break;
		case ToolManager::TERRAINTOOL:
		{
			objIter oiter = objects.begin();
			for (; oiter != objects.end(); oiter++)
			{
				modelIter miter = oiter->second.begin();
				for (; miter != oiter->second.end(); miter++)
					miter->second->Update();
			}

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
			{
				if (tools[curToolType]->GetTerrainPointer() == tIter->second)continue;
				tIter->second->Update();
			}

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Update();

			tools[curToolType]->Update();
		}
		break;
		case ToolManager::CAMERATOOL:
		{
			objIter oiter = objects.begin();
			for (; oiter != objects.end(); oiter++)
			{
				modelIter miter = oiter->second.begin();

				for (; miter != oiter->second.end(); miter++)
					miter->second->Update();
			}

			//터레인
			terrainIter tIter = terrains.begin();
			for (; tIter != terrains.end(); tIter++)
				tIter->second->Update();

			//물
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
				wIter->second->Update();

			tools[curToolType]->Update();
		}
		break;
	}

	CAM->Update();
	frustum->Update();

}

void ToolManager::PreRender()
{
	if (LIGHT->UseShadow())
		LIGHT->PreRender();
}

void ToolManager::PostRender()
{
	if (curToolType == OBJECTTOOL)
	{
		CharactorTool*tool = dynamic_cast<CharactorTool*>(tools[curToolType]);
		tool->DrawBoneNameText();
	}
}

void ToolManager::Release()
{
	objIter oiter = objects.begin();

	for (; oiter != objects.end(); oiter++)
	{
		modelIter miter = oiter->second.begin();
		for (; miter != oiter->second.end(); miter++)
		{
			SAFE_DELETE(miter->second);
		}
	}

	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		SAFE_DELETE(tIter->second);

	waterIter wIter = waters.begin();
	for (; wIter != waters.end(); wIter++)
		SAFE_DELETE(wIter->second);
}

void ToolManager::ExportFile(wstring filename)
{
	Fbx::Exporter*exporter = NULL;
	if (eType.isMaterial == true)
	{
		wstring saveFolderName = String::ToWString(savePath) + L"/";
		exporter = new Fbx::Exporter(filename);

		wstring fileName = String::ToWString(savePath) + L".material";
		exporter->ExportMaterial(Models + saveFolderName, fileName);

		SAFE_DELETE(exporter);
	}
	if (eType.isMesh == true)
	{
		wstring saveFolderName = String::ToWString(savePath) + L"/";
		exporter = new Fbx::Exporter(filename);

		wstring fileName = String::ToWString(savePath) + L".mesh";
		exporter->ExportMesh(Models + saveFolderName, fileName);

		SAFE_DELETE(exporter);
	}
	if (eType.isAnim == true)
	{
		wstring saveFolderName = String::ToWString(savePath) + L"/";
		exporter = new Fbx::Exporter(filename);

		vector<wstring> animFileName;

		String::SplitString(&animFileName, Path::GetFileName(filename), L".");
		wstring fileName = animFileName[0] + L".anim";
		exporter->ExportAnimation(Models + saveFolderName, fileName, 0);
		SAFE_DELETE(exporter);
	}
}

void ToolManager::LoadFile(wstring filename)
{
	wstring originFileName = Path::GetFileName(filename);

	vector<wstring> comp;
	String::SplitString(&comp, originFileName, L".");

	if (String::Contain(comp[1], L"material"))
	{
		Material*material = new Material;

		Xml::XMLDocument*document = new Xml::XMLDocument();

		wstring tempFile = filename;
		Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
		assert(error == Xml::XML_SUCCESS);

		Xml::XMLElement*root = document->FirstChildElement();
		Xml::XMLElement*matNode = root->FirstChildElement();

		do
		{
			Xml::XMLElement*node = NULL;
			Material*material = new Material();

			node = matNode->FirstChildElement();
			material->Name(String::ToWString(node->GetText()));

			node = node->NextSiblingElement();

			D3DXCOLOR dxColor;
			Xml::XMLElement*color = node->FirstChildElement();
			dxColor.r = color->FloatText();

			color = color->NextSiblingElement();
			dxColor.g = color->FloatText();

			color = color->NextSiblingElement();
			dxColor.b = color->FloatText();

			color = color->NextSiblingElement();
			dxColor.a = color->FloatText();
			material->SetDiffuse(dxColor);

			node = node->NextSiblingElement();
			wstring diffuseTexture = String::ToWString(node->GetText());

			//디렉토리이름 = 파일이름빼고 경로이름
			wstring directory = Path::GetDirectoryName(filename);
			if (diffuseTexture.length() > 0)
			{
				material->SetDiffuseMap(directory + diffuseTexture);
			}

			node = node->NextSiblingElement();
			if (node == NULL)
			{
				matVec.push_back(material);
				matNode = matNode->NextSiblingElement();
				continue;
			}

			wstring speculatTexture = String::ToWString(node->GetText());
			directory = Path::GetDirectoryName(filename);
			if (speculatTexture.length() > 0)
			{
				material->SetSpecularMap(directory + speculatTexture);
			}

			node = node->NextSiblingElement();
			if (node == NULL)
			{
				matVec.push_back(material);
				matNode = matNode->NextSiblingElement();
				continue;
			}

			wstring normalMap = String::ToWString(node->GetText());
			directory = Path::GetDirectoryName(filename);
			if (normalMap.length() > 0)
			{
				material->SetNormalMap(directory + normalMap);
			}
			material->SetShader(CharactorShader);
			matVec.push_back(material);
			matNode = matNode->NextSiblingElement();
		} while (matNode != NULL);
	}
	if (String::Contain(comp[1], L"mesh"))
	{
		vector<wstring> split;
		String::SplitString(&split, filename, L"/");

		wstring folderName = L"../" + split[2] + L"/" + split[3] + L"/";
		wstring fileName = split[4];

		vector<wstring> materialNames;
		String::SplitString(&materialNames, split[4], L".");

		SettingClassTag(CharactorShader, folderName, materialNames[0], fileName);
	}
	if (String::Contain(comp[1], L"anim"))
	{
		ModelClip*clip = new ModelClip(filename);
		animVec.push_back(clip);
	}
}

void ToolManager::LoadModelMapFile(class GameModel*model, wstring filename, int type, int idx)
{
	switch (type)
	{
		case 0:
		{
			model->GetModel()->Materials()[idx]->SetDiffuseMap(filename);
		}
		break;
		case 1:
		{
			model->GetModel()->Materials()[idx]->SetSpecularMap(filename);
		}
		break;
		case 2:
		{
			model->GetModel()->Materials()[idx]->SetNormalMap(filename);
		}
		break;
	}
}

void ToolManager::LoadTerrainMapFile(TerrainRender * tree, wstring filename, int type)
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

void ToolManager::ShowModelBone(wstring name, int index,wstring modelType)
{
	if (objects[modelType][name]->GetModel()->BoneByIndex(index) == NULL)return;

	if (index == 0)
	{
		if (objects[modelType][name]->GetModel()->BoneByIndex(index)->ChildCount() == 0)
			index = 1;
		else
			index = 0;
	}

	wstring boneName = objects[modelType][name]->GetModel()->BoneByIndex(index)->Name();
	if (ImGui::TreeNode(String::ToString(boneName).c_str()))
	{
		for (int i = 0; i < objects[modelType][name]->GetModel()->BoneByIndex(index)->GetChilds().size(); i++)
		{
			ShowModelBone(name, objects[modelType][name]->GetModel()->BoneByIndex(index)->Child(i)->Index(), modelType);
		}

		ImGui::TreePop();
	}
	else if (ImGui::BeginDragDropTarget()) // 오브젝트를 뼈에종속시킬때
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjPayload"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameModel));
			GameModel* data = (GameModel*)payload->Data;

			//전달된 데이터에 외부메트릭스 정보를 현재케릭터이름에 맞는 본의 로컬준다.
			modelIter miter = objects[L"Object"].begin();
			for (; miter != objects[L"Object"].end(); miter++)
			{
				if (miter->second->ModelName() == data->ModelName())
				{
					miter->second->SetParentModel(
						objects[L"Charactor"][name],
						objects[L"Charactor"][name]->GetModel()->BoneByIndex(index)->Name());
					break;
				}
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Collider"))//콜라이더를 뼈에종속시킬때
		{	
			IM_ASSERT(payload->DataSize == sizeof(Collider));
			Collider* data = (Collider*)payload->Data;

			modelIter miter = objects[L"Charactor"].find(name);

			for(int i=0; i<miter->second->GetColliders().size(); i++)
			{
				if (miter->second->GetColliders()[i]->GetColliderName() ==
					data->GetColliderName())
				{
					miter->second->GetColliders()[i]->SetGameModel(objects[L"Charactor"][name],
						objects[L"Charactor"][name]->GetModel()->BoneByIndex(index)->Name());
					break;
				}
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Trail"))
		{
			IM_ASSERT(payload->DataSize == sizeof(Trail));
			Trail* data = (Trail*)payload->Data;

			modelIter miter = objects[L"Charactor"].find(name);

			for (int i = 0; i<miter->second->GetTrails().size(); i++)
			{
				if (miter->second->GetTrails()[i]->GetTrailName() ==
					data->GetTrailName())
				{
					//본네임 셋팅해주고
					miter->second->GetTrails()[i]->SetBoneName(objects[L"Charactor"][name]->GetModel()->BoneByIndex(index)->Name());
					miter->second->GetTrails()[i]->GetParentName() = objects[L"Charactor"][name]->GetModel()->BoneByIndex(index)->Name();
					//부모이름 본네임으로 바꿔준다
					break;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


}

void ToolManager::PickedInit()
{
	objIter oiter = objects.begin();
	modelIter miter;

	for (; oiter != objects.end(); oiter++)
	{
		miter = oiter->second.begin();
		for (; miter != oiter->second.end(); miter++)
		{
			miter->second->PickState() = false;
		}
	}

	terrainIter tIter = terrains.begin();
	for (; tIter != terrains.end(); tIter++)
		tIter->second->IsPick() = false;

	waterIter wIter = waters.begin();
	for (; wIter != waters.end(); wIter++)
		wIter->second->IsPick() = false;
}

void ToolManager::PickSetting(bool value)
{
	objIter oiter = objects.begin();
	modelIter miter;

	for (; oiter != objects.end(); oiter++)
	{
		miter = oiter->second.begin();
		for (; miter != oiter->second.end(); miter++)
		{
			//TODO:게임모델쪽에서 막아버리는함수만들기
			//miter->second->GetColliderManater()->SetClickState(value);
		}
	}
}

void ToolManager::DrawAsset()
{
	ImGui::Begin("Asset", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	{
		ImGui::Separator();
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Set Export]");
		ImGui::Checkbox("Export Material", &eType.isMaterial);
		ImGui::Checkbox("Export Mesh", &eType.isMesh);
		ImGui::Checkbox("Export Animation", &eType.isAnim);
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
			for (int i = 0; i < tagNames.size(); i++)
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
		ImGui::Text("Drag Material Data : [%s]", String::ToString(dragMatName).c_str());

		if (dragMatName == L"")
			dragMatColor = { 1,0,0 };
		else
			dragMatColor = { 0,0,1 };

		ImGui::ColorButton("DragMaterial", ImVec4(dragMatColor.x, dragMatColor.y, dragMatColor.z, 1));
		ImGui::SameLine();

		if (ImGui::Button("PushMaterial"))
		{
			if (dragMatColor.z == 1)
			{
				for (int i = 0; i < matVec.size(); i++)
				{
					if (matVec[i]->Name() == dragMatName)
					{
						modelIter miter = objects[L"Charactor"].find(selectmodelName);
						if (miter == objects[L"Charactor"].end())break;

						if (matVec[i]->GetDiffuseMap() != NULL)
							miter->second->SetDiffuseMap(matVec[i]->GetDiffuseMap(),i);
						if (matVec[i]->GetSpecularMap() != NULL)
							miter->second->SetSpecularMap(matVec[i]->GetSpecularMap(),i);
						if (matVec[i]->GetNormalMap() != NULL)
							miter->second->SetNormalMap(matVec[i]->GetNormalMap(),i);
						break;
					}
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("PopMaterial"))
		{
			dragMatName = L"";
			matVec.pop_back();
		}
		if (ImGui::Button("AllPush",ImVec2(105,25)))
		{
			for (int i = 0; i < matVec.size(); i++)
			{
				modelIter miter = objects[L"Charactor"].find(selectmodelName);
				if (miter == objects[L"Charactor"].end())break;

				if (matVec[i]->GetDiffuseMap() != NULL)
					miter->second->SetDiffuseMap(matVec[i]->GetDiffuseMap(), i);
				if (matVec[i]->GetSpecularMap() != NULL)
					miter->second->SetSpecularMap(matVec[i]->GetSpecularMap(), i);
				if (matVec[i]->GetNormalMap() != NULL)
					miter->second->SetNormalMap(matVec[i]->GetNormalMap(), i);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("AllPop", ImVec2(105, 25)))
		{
			dragMatName = L"";
			matVec.clear();
		}

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Material Datas] : %d", matVec.size());
		ImGui::SameLine();
		if (ImGui::ArrowButton("mat", ImGuiDir_Down))
		{
			if (!eType.showMaterial)eType.showMaterial = true;
			else eType.showMaterial = false;
		}
		if (eType.showMaterial)
		{
			for (int i = 0; i < matVec.size(); i++)
			{
				if (ImGui::TreeNode((String::ToString(matVec[i]->Name())).c_str()))
				{
					dragMatName = matVec[i]->Name();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Material Name:%s", String::ToString(matVec[i]->Name()).c_str());
					ImGui::Separator();
					ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)", matVec[i]->GetDiffuse()->r, matVec[i]->GetDiffuse()->g, matVec[i]->GetDiffuse()->b);
					ImGui::Text("Diffuse Map");
					if (matVec[i]->GetDiffuseMap() == NULL)
						ImGui::Image(0, ImVec2(80, 80));
					else
						ImGui::Image(matVec[i]->GetDiffuseMap()->GetView(), ImVec2(80, 80));

					ImGui::Text("Specular Map");
					if (matVec[i]->GetSpecularMap() == NULL)
						ImGui::Image(0, ImVec2(80, 80));
					else
						ImGui::Image(matVec[i]->GetSpecularMap()->GetView(), ImVec2(80, 80));

					ImGui::Text("Normal Map");
					if (matVec[i]->GetNormalMap() == NULL)
						ImGui::Image(0, ImVec2(80, 80));
					else
						ImGui::Image(matVec[i]->GetNormalMap()->GetView(), ImVec2(80, 80));

					ImGui::TreePop();
				}
				else if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("MatPayload", matVec[i], sizeof(Material));
					ImGui::Text(String::ToString(matVec[i]->Name()).c_str());
					ImGui::EndDragDropSource();
				}
			}
		}

		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text("Drag Model Data : [%s]", String::ToString(dragModelName).c_str());

		if (dragModelName == L"")
			dragModelColor = { 1,0,0 };
		else
			dragModelColor = { 0,0,1 };

		ImGui::ColorButton("DragModel", ImVec4(dragModelColor.x, dragModelColor.y, dragModelColor.z, 1));
		ImGui::SameLine();
		if (ImGui::Button("PushModel"))
		{
			if (dragModelColor.z == 1)
			{
				for (int i = 0; i < modelVec.size(); i++)
				{
					if (modelVec[i]->ModelName() == dragModelName)
					{
						if (modelVec[i]->ClassName() == L"DefaultAnim")
						{
							GameAnimModel*copyModel = dynamic_cast<GameAnimModel*>(modelVec[i]);

							GameModel*model = new GameAnimModel(*copyModel);
							objects[L"Charactor"][dragModelName] = model;
							objects[L"Charactor"][dragModelName]->ModelName() = dragModelName;
							objects[L"Charactor"][dragModelName]->ClassName() = L"DefaultAnim";

							LIGHT->GetShadow()->Add(objects[L"Charactor"][dragModelName]);
							modelNames.push_back(dragModelName);
							break;
						}
						else if (modelVec[i]->ClassName() == L"DefaultModel")
						{
							GameModel*copyModel =modelVec[i];

							GameModel*model = new GameModel(*copyModel);
							objects[L"Object"][dragModelName] = model;
							objects[L"Object"][dragModelName]->ModelName() = dragModelName;
							objects[L"Object"][dragModelName]->ClassName() = L"DefaultModel";

							LIGHT->GetShadow()->Add(objects[L"Object"][dragModelName]);
							modelNames.push_back(dragModelName);
							break;
						}
						else if (modelVec[i]->ClassName() == L"Player")
						{
							Player*copyModel = dynamic_cast<Player*>(modelVec[i]);

							GameModel*model = new Player(*copyModel);
							objects[L"Charactor"][dragModelName] = model;
							objects[L"Charactor"][dragModelName]->ModelName() = dragModelName;
							objects[L"Charactor"][dragModelName]->ClassName() = L"Player";

							LIGHT->GetShadow()->Add(objects[L"Charactor"][dragModelName]);
							modelNames.push_back(dragModelName);
							break;
						}
						else if (modelVec[i]->ClassName() == L"Weapon")
						{
							WeaponModel*copyModel = dynamic_cast<WeaponModel*>(modelVec[i]);

							GameModel*model = new WeaponModel(*copyModel);
							objects[L"Object"][dragModelName] = model;
							objects[L"Object"][dragModelName]->ModelName() = dragModelName;
							objects[L"Object"][dragModelName]->ClassName() = L"Weapon";

							LIGHT->GetShadow()->Add(objects[L"Object"][dragModelName]);
							modelNames.push_back(dragModelName);
							break;
						}
					}
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("PopModel"))
		{
			dragModelName = L"";
			modelVec.pop_back();
		}

		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Model Datas]: %d", modelVec.size());
		ImGui::SameLine();
		if (ImGui::ArrowButton("model", ImGuiDir_Down))
		{
			if (!eType.showModel)eType.showModel = true;
			else eType.showModel = false;
		}
		if (eType.showModel)
		{
			for (int i = 0; i < modelVec.size(); i++)
			{
				if (ImGui::TreeNode((String::ToString(modelVec[i]->ModelName())).c_str()))
				{
					dragModelName = modelVec[i]->ModelName();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Model Name:%s", String::ToString(modelVec[i]->ModelName()).c_str());
					ImGui::Separator();
					ImGui::TextColored(ImVec4(0.7f, 0.0f, 0, 1), "[Bones]");
					ImGui::Text("Bone Count : %d", modelVec[i]->GetModel()->Bones().size());
					ImGui::Separator();
					ImGui::TextColored(ImVec4(0.7f, 0.0f, 0, 1), "[Materials]");
					ImGui::Text("Material Count : %d", modelVec[i]->GetModel()->Materials().size());
					for (int j = 0; j < modelVec[i]->GetModel()->Materials().size(); j++)
					{
						if (ImGui::TreeNode((String::ToString(modelVec[i]->GetModel()->Materials()[j]->Name())).c_str()))
						{
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Material Name:%s", String::ToString(modelVec[i]->GetModel()->Materials()[j]->Name()).c_str());
							ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)", modelVec[i]->GetModel()->Materials()[j]->GetDiffuse()->r, modelVec[i]->GetModel()->Materials()[j]->GetDiffuse()->g, modelVec[i]->GetModel()->Materials()[j]->GetDiffuse()->b);
							ImGui::Text("Diffuse Map");
							if (modelVec[i]->GetModel()->Materials()[j]->GetDiffuseMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(modelVec[i]->GetModel()->Materials()[j]->GetDiffuseMap()->GetView(), ImVec2(80, 80));
							}
							ImGui::Text("Specular Map");
							if (modelVec[i]->GetModel()->Materials()[j]->GetSpecularMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(modelVec[i]->GetModel()->Materials()[j]->GetSpecularMap()->GetView(), ImVec2(80, 80));
							}
							ImGui::Text("Normal Map");
							if (modelVec[i]->GetModel()->Materials()[j]->GetNormalMap() == NULL)
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0, 1), "[None]");
								ImGui::Image(0, ImVec2(0, 0));
							}
							else
							{
								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.0f, 0.0f, 1, 1), "[Exist]");
								ImGui::Image(modelVec[i]->GetModel()->Materials()[j]->GetNormalMap()->GetView(), ImVec2(80, 80));
							}
							ImGui::TreePop();
						}
					}
					ImGui::Separator();
					ImGui::TreePop();
				}
				else if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("ModelPayload", modelVec[i], sizeof(GameModel));
					ImGui::Text(String::ToString(modelVec[i]->ModelName()).c_str());
					ImGui::EndDragDropSource();

				}
			}
		}
		ImGui::Separator();
		ImGui::Separator();

		if (curToolType != TOOLTYPE::NONE)
			tools[curToolType]->AssetRender();

	}
	ImGui::End();
}

void ToolManager::DrawInspector()
{
	ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	{
		switch (curToolType)
		{
			case ToolManager::NONE:
			{
				//==================================오브젝트==============================//
				objIter oiter = objects.begin();
				modelIter miter;
				for (; oiter != objects.end(); oiter++)
				{
					miter = oiter->second.begin();
					for (; miter != oiter->second.end(); miter++)
					{
						if (miter->second->PickState())
						{
							ImGui::Checkbox("Visible", &(miter->second->isVisible));
							ImGui::SameLine();
							ImGui::Checkbox("Active", &(miter->second->isActive));
							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "=======[%s]=======", String::ToString(miter->second->ModelName()).c_str());

							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
							ImGui::Text("Center(%.2f,%.2f,%.2f)", miter->second->GetCenterPos().x, miter->second->GetCenterPos().y, miter->second->GetCenterPos().z);

							ImGui::Separator();
							ImGui::Separator();
							wstring textTrans = L"[" + miter->second->ModelName() + L"]translate";
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
							ImGui::Text(String::ToString(textTrans).c_str());
							ImGui::DragFloat3((String::ToString(miter->second->ModelName()) + "T").c_str(), (float*)&miter->second->Position(), 0.05f, -128, 128);
							miter->second->UpdateWorld();

							wstring textScale = L"[" + miter->second->ModelName() + L"]scale";
							ImGui::Text(String::ToString(textScale).c_str());
							ImGui::DragFloat3((String::ToString(miter->second->ModelName()) + "S").c_str(), (float*)&miter->second->Scale(), 0.05f, -10, 10);
							miter->second->UpdateWorld();

							wstring textRotate = L"[" + miter->second->ModelName() + L"]rotate";
							ImGui::Text(String::ToString(textRotate).c_str());
							ImGui::DragFloat3((String::ToString(miter->second->ModelName()) + "R").c_str(), (float*)&miter->second->Rotation(), 0.05f, -3.14f, 3.14f);
							miter->second->UpdateWorld();

							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
							if (ImGui::TreeNode("Diffuse"))
							{
								ImGui::Text("Model Diffuse");

								ImGui::ColorEdit4("d", miter->second->diffuseColor);
								{
									for (Material*material : miter->second->GetModel()->Materials())
										material->SetDiffuse(miter->second->diffuseColor);
								}

								ImGui::Text("Model DiffuseMap");
								for (int i = 0; i < miter->second->GetModel()->Materials().size(); i++)
								{
									if (miter->second->GetModel()->Materials()[i]->GetDiffuseMap() != NULL)
									{
										if (ImGui::ImageButton(miter->second->GetModel()->Materials()[i]->GetDiffuseMap()->GetView(), ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 0, i));
										}
									}
									else
									{
										if (ImGui::ImageButton(0, ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 0, i));
										}
									}
									ImGui::SameLine();
								}
								ImGui::TreePop();
							}

							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Specular]");
							if (ImGui::TreeNode("Specular"))
							{
								ImGui::Text("Model Specular");
								ImGui::ColorEdit4("s", miter->second->specularColor);
								{
									for (Material*material : miter->second->GetModel()->Materials())
										material->SetSpecular(miter->second->specularColor);
								}
								ImGui::Text("Shininess");
								ImGui::DragFloat("", &miter->second->shiness, 1, 1, 50);
								{
									for (Material*material : miter->second->GetModel()->Materials())
										material->SetShininess(miter->second->shiness);
								}

								ImGui::Text("Model SpecularMap");
								for (int i = 0; i < miter->second->GetModel()->Materials().size(); i++)
								{
									if (miter->second->GetModel()->Materials()[i]->GetSpecularMap() != NULL)
									{
										if (ImGui::ImageButton(miter->second->GetModel()->Materials()[i]->GetSpecularMap()->GetView(), ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 1, i));
										}
									}
									else
									{
										if (ImGui::ImageButton(0, ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 1, i));
										}
									}
									ImGui::SameLine();
								}
								ImGui::TreePop();
							}

							ImGui::Separator();
							ImGui::Separator();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set NormalMap]");
							if (ImGui::TreeNode("NormalMap"))
							{
								if (ImGui::Button("Use NormalMap"))
								{
									if (miter->second->normalBuffer->Data.isVisible == 1)
										miter->second->normalBuffer->Data.isVisible = 0;
									else
										miter->second->normalBuffer->Data.isVisible = 1;
								}

								ImGui::Text("Model NormalMap");
								for (int i = 0; i < miter->second->GetModel()->Materials().size(); i++)
								{
									if (miter->second->GetModel()->Materials()[i]->GetNormalMap() != NULL)
									{
										if (ImGui::ImageButton(miter->second->GetModel()->Materials()[i]->GetNormalMap()->GetView(), ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 2, i));
										}
									}
									else
									{
										if (ImGui::ImageButton(0, ImVec2(100, 100)))
										{
											Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadModelMapFile, this, miter->second, placeholders::_1, 2, i));
										}
									}
									ImGui::SameLine();
								}
								ImGui::TreePop();
							}
							ImGui::Separator();
							ImGui::Separator();
						
							miter->second->ColliderSetRender();
							miter->second->TrailSetRender();

							ImGui::Separator();
							ImGui::Separator();
						}
					}
				}
				//========================================================================//


				//======================================터레인=====================================//
				terrainIter tIter = terrains.begin();
				for (; tIter != terrains.end(); tIter++)
				{
					if (tIter->second->IsPick())
					{
						ImGui::Checkbox("Visible", &(tIter->second->isVisible));
						ImGui::SameLine();
						ImGui::Checkbox("Active", &(tIter->second->isActive));
						ImGui::Separator();
						ImGui::Separator();
						ImGui::TextColored(ImVec4(1, 1, 0, 1), "=======[%s]=======", String::ToString(tIter->first).c_str());

						ImGui::Separator();
						ImGui::Separator();
						wstring textTrans = L"[" + tIter->first + L"]translate";
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
						ImGui::Text(String::ToString(textTrans).c_str());
						ImGui::DragFloat3((String::ToString(tIter->first) + "T").c_str(), (float*)&tIter->second->Position(), 0.05f, -128, 128);
						tIter->second->UpdateMatrix();

						wstring textScale = L"[" + tIter->first + L"]scale";
						ImGui::Text(String::ToString(textScale).c_str());
						ImGui::DragFloat3((String::ToString(tIter->first) + "S").c_str(), (float*)&tIter->second->Scale(), 0.05f, -10, 10);
						tIter->second->UpdateMatrix();

						wstring textRotate = L"[" + tIter->first + L"]rotate";
						ImGui::Text(String::ToString(textRotate).c_str());
						ImGui::DragFloat3((String::ToString(tIter->first) + "R").c_str(), (float*)&tIter->second->Rotate(), 0.05f, -3.14f, 3.14f);
						tIter->second->UpdateMatrix();

						ImGui::Separator();
						ImGui::Separator();

						ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Diffuse]");
						if (ImGui::TreeNode("Diffuse"))
						{
							ImGui::Text("Model DiffuseMap");
							if (tIter->second->GetMaterial()->GetDiffuseMap() != NULL)
							{
								if (ImGui::ImageButton(tIter->second->GetMaterial()->GetDiffuseMap()->GetView(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 0));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 0));
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

							if (tIter->second->GetMaterial()->GetSpecularMap() != NULL)
							{
								if (ImGui::ImageButton(tIter->second->GetMaterial()->GetSpecularMap()->GetView(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 1));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 1));
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

							if (tIter->second->GetMaterial()->GetNormalMap() != NULL)
							{
								if (ImGui::ImageButton(tIter->second->GetMaterial()->GetNormalMap()->GetView(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 2));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 2));
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

							if (tIter->second->GetMaterial()->GetNormalMap() != NULL)
							{
								if (ImGui::ImageButton(tIter->second->GetMaterial()->GetDetailMap()->GetView(), ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 3));
								}
							}
							else
							{
								if (ImGui::ImageButton(0, ImVec2(100, 100)))
								{
									Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ToolManager::LoadTerrainMapFile, this, tIter->second, placeholders::_1, 3));
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

				//=======================================물==========================================//
				waterIter wIter = waters.begin();
				for (; wIter != waters.end(); wIter++)
				{
					if (wIter->second->IsPick())
					{
						wIter->second->ImGuiRender();
					}
				}
				//==================================================================================//

			}
			break;
			case ToolManager::OBJECTTOOL:
			{
				tools[curToolType]->InspectRender();
			}
			break;
			case ToolManager::TERRAINTOOL:
			{
				tools[curToolType]->InspectRender();
			}
			break;
			case ToolManager::CAMERATOOL:
			{
				tools[curToolType]->InspectRender();
			}
			break;
		}
		
	}
	ImGui::End();
}

void ToolManager::DrawHiarachy()
{
	ImGui::Begin("Hiarachy", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "============[Game Object]============");

		//=================================모델콤보창===================================//
		if (ImGui::BeginCombo("Models", String::ToString(selectmodelName).c_str())) 
		{
			for (int i = 0; i < modelNames.size(); i++)
			{
				bool isSelected = (selectmodelName == modelNames[i]);
				if (ImGui::Selectable(String::ToString(modelNames[i]).c_str(), isSelected))
				{
					selectmodelName = modelNames[i];
				}
				if (isSelected == true)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		else if (ImGui::BeginDragDropTarget())
		{
			//콤보창으로 모델끌어당기면 복사
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelPayload"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameModel));
				GameModel* data = (GameModel*)payload->Data;

				for (int i = 0; i < modelVec.size(); i++)
				{
					if (modelVec[i]->ModelName() == data->ModelName())
					{
						if (modelVec[i]->ClassName() == L"DefaultAnim")
						{
							GameAnimModel*copyModel = dynamic_cast<GameAnimModel*>(modelVec[i]);

							GameModel*model = new GameAnimModel(*copyModel);
							objects[L"Charactor"][modelVec[i]->ModelName()] = model;
							objects[L"Charactor"][modelVec[i]->ModelName()]->ModelName() = data->ModelName();

							LIGHT->GetShadow()->Add(objects[L"Charactor"][modelVec[i]->ModelName()]);
							break;
						}
						else if (modelVec[i]->ClassName() == L"DefaultModel")
						{
							GameModel*copyModel = modelVec[i];

							GameModel*model = new GameModel(*copyModel);
							objects[L"Object"][modelVec[i]->ModelName()] = model;
							objects[L"Object"][modelVec[i]->ModelName()]->ModelName() = data->ModelName();

							LIGHT->GetShadow()->Add(objects[L"Object"][modelVec[i]->ModelName()]);
							break;
						}
					}
				}
				for (int i = 0; i < modelNames.size(); i++)
				{
					if (modelNames[i] == data->ModelName())
					{
						modelNames.pop_back();
						break;
					}
				}
				modelNames.push_back(data->ModelName());
			}
			ImGui::EndDragDropTarget();
		}
		//===================================================================================//

		if (ImGui::Button("COPY",ImVec2(95,20)))
		{
			static int copyModelNum = 0;
			modelIter miter = objects[L"Charactor"].find(selectmodelName);

			if (miter != objects[L"Charactor"].end())
			{
				GameAnimModel*downCast = dynamic_cast<GameAnimModel*>(objects[L"Charactor"][selectmodelName]);
				GameModel*copyModel = new GameAnimModel(*downCast);

				copyModel->ClassName() = downCast->ClassName();
				copyModel->ModelName() = downCast->ModelName() + L"C" + to_wstring(copyModelNum);
				copyModelNum++;

				objects[L"Charactor"][copyModel->ModelName()] = copyModel;
				LIGHT->GetShadow()->Add(objects[L"Charactor"][copyModel->ModelName()]);

				modelNames.push_back(copyModel->ModelName());

				if (downCast->GetAnimClip().size() > 0)
				{
					CharactorTool*tempTool = dynamic_cast<CharactorTool*>(tools[TOOLTYPE::OBJECTTOOL]);
					GameAnimModel*tempAnim = dynamic_cast<GameAnimModel*>(tempTool->GetModelPointer());
					for (int i = 0; i <tempAnim->GetAnimClip().size(); i++)
					{
						wstring animName = tempAnim->GetAnimClip()[i]->GetName();
						tempTool->InputAnimNames(copyModel->ModelName(), animName);
					}
				}
			}

			miter = objects[L"Object"].find(selectmodelName);
			if (miter != objects[L"Object"].end())
			{
				GameModel*copyModel = new GameModel(*objects[L"Object"][selectmodelName]);

				copyModel->ClassName() = objects[L"Object"][selectmodelName]->ClassName();
				copyModel->ModelName() = objects[L"Object"][selectmodelName]->ModelName() + L"C" + to_wstring(copyModelNum);
				copyModelNum++;

				objects[L"Object"][copyModel->ModelName()] = copyModel;
				LIGHT->GetShadow()->Add(objects[L"Object"][copyModel->ModelName()]);

				modelNames.push_back(copyModel->ModelName());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("DELETE", ImVec2(95, 20)))
		{
			objIter oiter = objects.begin();
			modelIter miter;

			for (; oiter != objects.end(); oiter++)
			{
				miter = oiter->second.find(selectmodelName);
				
				
				if (miter != oiter->second.end())
				{
					LIGHT->GetShadow()->Erase(selectmodelName);
					SAFE_DELETE(miter->second);
					oiter->second.erase(miter);

					break;
				}
			}

			terrainIter tIter = terrains.find(selectmodelName);
			if (tIter != terrains.end())
			{
				SAFE_DELETE(tIter->second);
				terrains.erase(tIter);
			}

			waterIter wIter = waters.find(selectmodelName);
			if (wIter != waters.end())
			{
				SAFE_DELETE(wIter->second);
				waters.erase(wIter);
			}

			for (int i = 0; i < modelNames.size(); i++)
			{
				if (modelNames[i] == selectmodelName)
				{
					modelNames.erase(modelNames.begin()+i);
					break;
				}
			}
			selectmodelName = L"";
		}

		ImGui::Separator();
		ImGui::Separator();

		//======================================오브젝트===================================//
		//======================================케릭터====================================//
		if (ImGui::SmallButton("Charactor"))
		{
			if (!eType.isShowComboCharactor)eType.isShowComboCharactor = true;
			else eType.isShowComboCharactor = false;
		}
		modelIter miter;

		ImGui::Separator();
		if (eType.isShowComboCharactor)
		{
			miter = objects[L"Charactor"].begin();
			for (; miter != objects[L"Charactor"].end(); miter++)
			{
				if (ImGui::TreeNode(String::ToString(miter->second->ModelName()).c_str()))
				{
					//이전에 클릭한것들전부 false로만들고 클릭한애true
					PickedInit();
					miter->second->PickState() = true;
					if (selectmodelName != miter->second->ModelName())
						selectmodelName = miter->second->ModelName();

					//뼈가 2개이상 없는모델은 뺀다
					if (miter->second->GetModel()->BoneCount() < 2)
						ImGui::TreePop();
					else
					{
						ShowModelBone(miter->second->ModelName(), miter->second->GetModel()->BoneByIndex(0)->Index(),L"Charactor");
						ImGui::TreePop();
					}
				}
				else if (ImGui::BeginDragDropSource())//하이라키모델 다른쪽에 넣을때 옵션
				{
					if (miter->second->ClassName() == L"DefaultAnim")
					{
						ImGui::SetDragDropPayload("ObjPayload", miter->second, sizeof(GameModel));
						ImGui::Text(String::ToString(miter->second->ModelName()).c_str());
						ImGui::EndDragDropSource();
					}
					else if (miter->second->ClassName() == L"Player")
					{
						ImGui::SetDragDropPayload("ObjPayload", miter->second, sizeof(GameModel));
						ImGui::Text(String::ToString(miter->second->ModelName()).c_str());
						ImGui::EndDragDropSource();
					}
					//추가되는 오브젝트들은 여기서 처리
				}
				else if (ImGui::BeginDragDropTarget())//머터리얼 받을 타겟
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MatPayload"))
					{
						IM_ASSERT(payload->DataSize == sizeof(Material));
						Material* data = (Material*)payload->Data;

						for (int i = 0; i < matVec.size(); i++)
						{
							if (matVec[i]->Name() == data->Name())
							{
								modelIter miter = objects[L"Charactor"].find(selectmodelName);
								if (miter == objects[L"Charactor"].end())break;

								if (matVec[i]->GetDiffuseMap() != NULL)
									miter->second->SetDiffuseMap(matVec[i]->GetDiffuseMap(), i);
								if (matVec[i]->GetSpecularMap() != NULL)
									miter->second->SetSpecularMap(matVec[i]->GetSpecularMap(), i);
								if (matVec[i]->GetNormalMap() != NULL)
									miter->second->SetNormalMap(matVec[i]->GetNormalMap(), i);
								break;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		}
		ImGui::Separator();
		//==================================================================================//

		//====================================오브젝트=====================================//
		if (ImGui::SmallButton("Object"))
		{
			if (!eType.isShowComboObject)eType.isShowComboObject = true;
			else eType.isShowComboObject = false;
		}

		ImGui::Separator();
		if (eType.isShowComboObject)
		{
			miter = objects[L"Object"].begin();
			for (; miter != objects[L"Object"].end(); miter++)
			{
				if (ImGui::TreeNode(String::ToString(miter->second->ModelName()).c_str()))
				{
					//이전에 클릭한것들전부 false로만들고 클릭한애true
					PickedInit();
					miter->second->PickState() = true;
					if (selectmodelName != miter->second->ModelName())
						selectmodelName = miter->second->ModelName();

					//뼈가 2개이상 없는모델은 뺀다
					if (miter->second->GetModel()->BoneCount() < 2)
						ImGui::TreePop();
					else
					{
						ShowModelBone(miter->second->ModelName(), miter->second->GetModel()->BoneByIndex(0)->Index(), L"Object");
						ImGui::TreePop();
					}
				}
				else if (ImGui::BeginDragDropSource())//하이라키모델 다른쪽에 넣을때 옵션
				{
					if (miter->second->ClassName() == L"DefaultModel")
					{
						ImGui::SetDragDropPayload("ObjPayload", miter->second, sizeof(GameModel));
						ImGui::Text(String::ToString(miter->second->ModelName()).c_str());
						ImGui::EndDragDropSource();
					}
					else if (miter->second->ClassName() == L"Weapon")
					{
						ImGui::SetDragDropPayload("ObjPayload", miter->second, sizeof(GameModel));
						ImGui::Text(String::ToString(miter->second->ModelName()).c_str());
						ImGui::EndDragDropSource();
					}
					//추가되는 오브젝트들은 여기서 처리
				}
				else if (ImGui::BeginDragDropTarget())//머터리얼 받을 타겟
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MatPayload"))
					{
						IM_ASSERT(payload->DataSize == sizeof(Material));
						Material* data = (Material*)payload->Data;

						for (int i = 0; i < matVec.size(); i++)
						{
							if (matVec[i]->Name() == data->Name())
							{
								modelIter miter = objects[L"Object"].find(selectmodelName);
								if (miter == objects[L"Object"].end())break;

								if (matVec[i]->GetDiffuseMap() != NULL)
									miter->second->SetDiffuseMap(matVec[i]->GetDiffuseMap(), i);
								if (matVec[i]->GetSpecularMap() != NULL)
									miter->second->SetSpecularMap(matVec[i]->GetSpecularMap(), i);
								if (matVec[i]->GetNormalMap() != NULL)
									miter->second->SetNormalMap(matVec[i]->GetNormalMap(), i);
								break;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		}
		ImGui::Separator();
		//==================================================================================//

		if (ImGui::SmallButton("LandScape"))
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
				if (ImGui::TreeNode(String::ToString(tIter->first).c_str()))
				{
					PickedInit();
					tIter->second->IsPick() = true;
					if (selectmodelName != tIter->first)
						selectmodelName = tIter->first;
					ImGui::TreePop();
				}
			}
			//==================================================================================//

			//======================================물=====================================//
			waterIter wIter = waters.begin();
			for (; wIter != waters.end(); wIter++)
			{
				if (ImGui::TreeNode(String::ToString(wIter->first).c_str()))
				{
					PickedInit();
					wIter->second->IsPick() = true;
					if (selectmodelName != wIter->first)
						selectmodelName = wIter->first;
					ImGui::TreePop();
				}
			}
			//==================================================================================//
		}
		ImGui::Separator();
	}
	ImGui::End();
}

void ToolManager::SettingClassTag(Shader*shader,wstring folderName, wstring matName, wstring fileName)
{
	static int toIndex = 0;

	//TODO:여기 플레이어클래스만들면 처리하기

	if (selectTagName == L" DefaultAnim")
	{
		GameModel*model = new GameAnimModel(folderName, matName + L".material",
			folderName, fileName, values);
		model->SetShader(shader);
		wstring toName = fileName + to_wstring(toIndex);
		model->ModelName() = toName;
		modelVec.push_back(model);
		//return;
	}
	else if (selectTagName == L" DefaultModel")
	{
		GameModel*model = new GameModel(folderName, matName + L".material",
			folderName, fileName, values);
		model->SetShader(shader);
		wstring toName = fileName + to_wstring(toIndex);
		model->ModelName() = toName;
		modelVec.push_back(model);
		//return;
	}
	else if (selectTagName == L" Player")
	{
		GameModel*model = new Player(folderName, matName + L".material",
			folderName, fileName, values);
		model->SetShader(shader);
		wstring toName = fileName + to_wstring(toIndex);
		model->ModelName() = toName;
		modelVec.push_back(model);
		//return;
	}
	else if (selectTagName == L" Weapon")
	{
		GameModel*model = new WeaponModel(folderName, matName + L".material",
			folderName, fileName, values);
		model->SetShader(shader);
		wstring toName = fileName + to_wstring(toIndex);
		model->ModelName() = toName;
		modelVec.push_back(model);
	}
	toIndex++;
}

