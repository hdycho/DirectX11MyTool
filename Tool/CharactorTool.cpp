#include "stdafx.h"
#include "CharactorTool.h"
#include "../Fbx/Exporter.h"
#include "../Utilities/Xml.h"
#include "../Objects/GameModel.h"
#include "../Objects/GameAnimModel.h"
#include "../Objects/BoneSphere.h"
#include "../Model/ModelBone.h"
#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"
#include "../Physics/ColliderBox.h"
#include "../Physics/ColliderCapsule.h"
#include "../Physics/Trail.h"


CharactorTool::CharactorTool(ExecuteValues*values)
	:GameTool(L"CharactorTool")
{
	animVec = NULL;
	sType = SET_NONE;
	this->values = values;
	selectAnimName = L"";
	colliderName = L"NONE"; // 이부분수정 초기받아오는값으로 이름바꿔야함
	//초기화함수만들자

	isRepeat = false;
	blendTime = 0.0f;
	startTime = 0.0f;
	speed = 1.0f;

	selectColliders.push_back(L"NONE");
	selectColliders.push_back(L"BOX");
	selectColliders.push_back(L"CAPSULE");
}

CharactorTool::~CharactorTool()
{
}


void CharactorTool::Update()
{
	wstring shaderFileName;
	if (modelType == L"DefaultAnim")
		shaderFileName = Path::GetFileName(aModel->GetModel()->Materials()[0]->GetShader()->GetFile());
	else
		shaderFileName = Path::GetFileName(nModel->GetModel()->Materials()[0]->GetShader()->GetFile());
	shaderName = shaderFileName;
	switch (sType)
	{
		case CharactorTool::SET_NONE:
		{
			selectAnimName = L"";
		}
		break;
		case CharactorTool::SET_MESH:
		{
			if (modelType == L"DefaultAnim")
				aModel->Update();
			else
				nModel->Update();
		}
		break;
		case CharactorTool::SET_ANIMATION:
		{
			if (modelType == L"DefaultAnim")
			{
				aModel->Update();
				bonesIter miter1 = bonesMap.find(aModel->ModelName());
				bonesScaleIter mster1 = bonesScaleMap.find(aModel->ModelName());

				for (int i = 0; i < miter1->second.size(); i++)
				{
					if (miter1->second[i]->PickState())
						miter1->second[i]->SetDiffuse(D3DXCOLOR(0, 1, 0, 0));
					else
						miter1->second[i]->SetDiffuse(D3DXCOLOR(1, 0, 0, 0));
					D3DXMATRIX changeMat = miter1->second[i]->GetOutMatrix();
					aModel->GetModel()->Bones()[i]->Local(changeMat, true);

					D3DXMATRIX result = aModel->GetBoneMatrix(i);

					miter1->second[i]->Position(result._41, result._42, result._43);
					miter1->second[i]->Scale(mster1->second[i].x*aModel->Scale().x, mster1->second[i].y*aModel->Scale().y, mster1->second[i].z*aModel->Scale().z);


					miter1->second[i]->Update();
				}
			}
		}
		break;
	}
}

void CharactorTool::Render()
{
	if (modelType == L"DefaultAnim")
		aModel->Render();
	else
		nModel->Render();
}

void CharactorTool::HirakyRender()
{
}

void CharactorTool::AssetRender()
{
	ImGui::Text("Drag Clip Data : [%s]", String::ToString(dragAnimName).c_str());

	if (dragAnimName == L"")
		dragAnimColor = { 1,0,0 };
	else
		dragAnimColor = { 0,0,1 };

	ImGui::ColorButton("DragClip", ImVec4(dragAnimColor.x, dragAnimColor.y, dragAnimColor.z, 1));
	ImGui::SameLine();
	if (ImGui::Button("PushClip"))
	{
		if (dragAnimColor.z == 1)
		{
			for (int i = 0; i < (*animVec).size(); i++)
			{
				if ((*animVec)[i]->GetName() == dragAnimName)
				{
					ModelClip*newClip = new ModelClip(*(*animVec)[i]);
					aModel->GetAnimClip().push_back(newClip);
					animNames[aModel->ModelName()].push_back(newClip->GetName());
					break;
				}
			}
			for (int i = 0; i < animNames[aModel->ModelName()].size(); i++)
			{
				if (animNames[aModel->ModelName()][i] == dragAnimName)
				{
					animNames[aModel->ModelName()].pop_back();
					break;
				}
			}
			animNames[aModel->ModelName()].push_back(dragAnimName);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("PopClip"))
	{
		dragAnimName = L"";
		(*animVec).pop_back();
	}
	if (ImGui::Button("AllPushClip"))
	{
		for (int i = 0; i < (*animVec).size(); i++)
		{
			ModelClip*newClip = new ModelClip(*(*animVec)[i]);
			aModel->GetAnimClip().push_back(newClip);
			animNames[aModel->ModelName()].push_back(newClip->GetName());
		}
	}

	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "[Animation Datas]: %d", (*animVec).size());
	//ImGui::SameLine();

	for (int i = 0; i < (*animVec).size(); i++)
	{
		if (ImGui::TreeNode((String::ToString((*animVec)[i]->GetName())).c_str()))
		{
			dragAnimName = (*animVec)[i]->GetName();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1), "Animation Name:%s", String::ToString((*animVec)[i]->GetName()).c_str());
			ImGui::Separator();
			ImGui::Text("Animation Info");
			ImGui::BulletText("Duration : %f(millisecond)", (*animVec)[i]->Duration());
			ImGui::BulletText("FrameCount : %d", (*animVec)[i]->FrameCount());
			ImGui::TreePop();
		}
		else if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ClipPayload", (*animVec)[i], sizeof(ModelClip));
			ImGui::Text(String::ToString((*animVec)[i]->GetName()).c_str());
			ImGui::EndDragDropSource();
		}
	}

	ImGui::Separator();
	ImGui::Separator();
}

void CharactorTool::InspectRender()
{
	if (ImGui::Button("SetShader"))
	{
		Path::OpenFileDialog(L"파일", L".hlsl", Shaders, bind(&CharactorTool::LoadShaderFile, this, placeholders::_1));
	}
	ImGui::Text("ShaderName : %s", String::ToString(shaderName).c_str());
	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Setting Type]");
	ImGui::RadioButton("NONE", (int*)&sType, SET_NONE);
	ImGui::SameLine();
	ImGui::RadioButton("MESH", (int*)&sType, SET_MESH);
	ImGui::SameLine();
	if (modelType == L"DefaultAnim")
	{
		ImGui::SameLine();
		ImGui::RadioButton("ANIMATION", (int*)&sType, SET_ANIMATION);
	}

	ImGui::Separator();
	ImGui::Separator();
	switch (sType)
	{
		case CharactorTool::SET_NONE:
		{
			if (modelType == L"DefaultAnim")
				aModel->Render();
			else
				nModel->Render();
		}
		break;
		case CharactorTool::SET_MESH:
		{
			if (modelType == L"DefaultAnim")
			{
				aModel->Render();
				if (aModel->PickState())
				{
					ImGui::Checkbox("Visible", &(aModel->isVisible));
					ImGui::SameLine();
					ImGui::Checkbox("Active", &(aModel->isActive));
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "=========[%s]=========", String::ToString(aModel->ModelName()).c_str());
					ImGui::BulletText("SkinedModel");
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
					ImGui::Text("Center(%.2f,%.2f,%.2f)", aModel->GetCenterPos().x, aModel->GetCenterPos().y, aModel->GetCenterPos().z);

					ImGui::Separator();
					ImGui::Separator();
					wstring textTrans = L"[" + aModel->ModelName() + L"]translate";
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
					ImGui::Text(String::ToString(textTrans).c_str());
					ImGui::DragFloat3((String::ToString(aModel->ModelName()) + "T").c_str(), (float*)&aModel->Position(), 0.05f, -128, 128);
					aModel->UpdateWorld();

					wstring textScale = L"[" + aModel->ModelName() + L"]scale";
					ImGui::Text(String::ToString(textScale).c_str());
					ImGui::DragFloat3((String::ToString(aModel->ModelName()) + "S").c_str(), (float*)&aModel->Scale(), 0.05f, -10, 10);
					aModel->UpdateWorld();

					wstring textRotate = L"[" + aModel->ModelName() + L"]rotate";
					ImGui::Text(String::ToString(textRotate).c_str());
					ImGui::DragFloat3((String::ToString(aModel->ModelName()) + "R").c_str(), (float*)&aModel->Rotation(), 0.05f, -3.14f, 3.14f);
					aModel->UpdateWorld();

					ImGui::Separator();
					ImGui::Separator();

					//콜라이더셋팅하는거 추가
					ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Colliders]");
					if (ImGui::BeginCombo("collider", String::ToString(colliderName).c_str()))
					{
						for (int i = 0; i < selectColliders.size(); i++)
						{
							bool isSelected = (colliderName == selectColliders[i]);
							if (ImGui::Selectable(String::ToString(selectColliders[i]).c_str(), isSelected))
							{
								colliderName = selectColliders[i];
							}
							if (isSelected == true)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					if (ImGui::Button("Create Collider"))
					{
						if (colliderName == L"BOX")
						{
							static int BoxNum = 0;
							Collider*collider = new ColliderBox(aModel->World(), COLLIDERSTATE::BOX, false);
							collider->InputColliderInfo(&aModel->InitBoxPos(), &aModel->CurBoxPos(), &aModel->MinmaxPos());
							collider->ColliderLineInit();

							collider->GetColliderName() = L"Box" + to_wstring(BoxNum);

							aModel->GetColliders().push_back(collider);
							BoxNum++;
						}
						else if (colliderName == L"CAPSULE")
						{
							static int CapsuleNum = 0;
							Collider*collider = new ColliderCapsule(aModel->World(), COLLIDERSTATE::CAPSULE, false);
							collider->InputColliderInfo(&aModel->InitBoxPos(), &aModel->CurBoxPos(), &aModel->MinmaxPos());
							collider->ColliderLineInit();

							collider->GetColliderName() = L"Capsule" + to_wstring(CapsuleNum);

							aModel->GetColliders().push_back(collider);
							CapsuleNum++;
						}
					}
					aModel->ColliderSetRender();

					ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Trail]");
					if (ImGui::Button("Create Trail"))
					{
						static int TrailNum = 0;
						Trail*trail = new Trail();
						trail->GetTrailName() = L"Trail" + to_wstring(TrailNum);
						trail->SetGameModel(aModel);


						aModel->GetTrails().push_back(trail);
						TrailNum++;
					}
					aModel->TrailSetRender();
					ImGui::Separator();
					ImGui::Separator();
				}
			}
			else
			{
				nModel->Render();
				if (nModel->PickState())
				{
					ImGui::Checkbox("Visible", &(nModel->isVisible));
					ImGui::SameLine();
					ImGui::Checkbox("Active", &(nModel->isActive));
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "=========[%s]=========", String::ToString(nModel->ModelName()).c_str());
					ImGui::BulletText("LegacyModel");
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Center Position]");
					ImGui::Text("Center(%.2f,%.2f,%.2f)", nModel->GetCenterPos().x, nModel->GetCenterPos().y, nModel->GetCenterPos().z);

					ImGui::Separator();
					ImGui::Separator();
					wstring textTrans = L"[" + nModel->ModelName() + L"]translate";
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
					ImGui::Text(String::ToString(textTrans).c_str());
					ImGui::DragFloat3((String::ToString(nModel->ModelName()) + "T").c_str(), (float*)&nModel->Position(), 0.05f, -128, 128);
					nModel->UpdateWorld();

					wstring textScale = L"[" + nModel->ModelName() + L"]scale";
					ImGui::Text(String::ToString(textScale).c_str());
					ImGui::DragFloat3((String::ToString(nModel->ModelName()) + "S").c_str(), (float*)&nModel->Scale(), 0.05f, -10, 10);
					nModel->UpdateWorld();

					wstring textRotate = L"[" + nModel->ModelName() + L"]rotate";
					ImGui::Text(String::ToString(textRotate).c_str());
					ImGui::DragFloat3((String::ToString(nModel->ModelName()) + "R").c_str(), (float*)&nModel->Rotation(), 0.05f, -3.14f, 3.14f);
					nModel->UpdateWorld();

					ImGui::Separator();
					ImGui::Separator();

					//콜라이더셋팅하는거 추가
					ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Colliders]");
					if (ImGui::BeginCombo("collider", String::ToString(colliderName).c_str()))
					{
						for (int i = 0; i < selectColliders.size(); i++)
						{
							bool isSelected = (colliderName == selectColliders[i]);
							if (ImGui::Selectable(String::ToString(selectColliders[i]).c_str(), isSelected))
							{
								colliderName = selectColliders[i];
							}
							if (isSelected == true)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					if (ImGui::Button("Create Collider"))
					{
						if (colliderName == L"BOX")
						{
							static int BoxNum = 0;
							Collider*collider = new ColliderBox(nModel->World(), COLLIDERSTATE::BOX, false);
							collider->InputColliderInfo(&nModel->InitBoxPos(), &nModel->CurBoxPos(), &nModel->MinmaxPos());
							collider->ColliderLineInit();

							collider->GetColliderName() = L"Box" + to_wstring(BoxNum);

							nModel->GetColliders().push_back(collider);
							BoxNum++;
						}
						else if (colliderName == L"CAPSULE")
						{
							static int CapsuleNum = 0;
							Collider*collider = new ColliderCapsule(nModel->World(), COLLIDERSTATE::CAPSULE, false);
							collider->InputColliderInfo(&nModel->InitBoxPos(), &nModel->CurBoxPos(), &nModel->MinmaxPos());
							collider->ColliderLineInit();

							collider->GetColliderName() = L"Capsule" + to_wstring(CapsuleNum);

							nModel->GetColliders().push_back(collider);
							CapsuleNum++;
						}
					}
					nModel->ColliderSetRender();

					ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Trail]");
					if (ImGui::Button("Create Trail"))
					{
						static int TrailNum = 0;
						Trail*trail = new Trail();
						trail->GetTrailName()= L"Trail" + to_wstring(TrailNum);
						trail->SetGameModel(nModel);
						
					
						nModel->GetTrails().push_back(trail);
						TrailNum++;
					}
					nModel->TrailSetRender();
					ImGui::Separator();
					ImGui::Separator();
				}
			}
		}
		break;
		case CharactorTool::SET_ANIMATION:
		{
			if (modelType == L"DefaultAnim")
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "=========[Animation]=========");
				ImGui::BulletText("SkinedModel");
				ImGui::Separator();
				ImGui::Separator();

				ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Animation Select]");
				aModel->Render();
				if (ImGui::BeginCombo("Clips", String::ToString(selectAnimName).c_str()))
				{
					for (int i = 0; i < animNames[aModel->ModelName()].size(); i++)
					{
						bool isSelected = (selectAnimName == animNames[aModel->ModelName()][i]);
						if (ImGui::Selectable(String::ToString(animNames[aModel->ModelName()][i]).c_str(), isSelected))
						{
							selectAnimName = animNames[aModel->ModelName()][i];
						}
						if (isSelected == true)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				else if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ClipPayload"))
					{
						IM_ASSERT(payload->DataSize == sizeof(ModelClip));
						ModelClip* data = (ModelClip*)payload->Data;

						for (int i = 0; i < (*animVec).size(); i++)
						{
							if ((*animVec)[i]->GetName() == data->GetName())
							{
								aModel->GetAnimClip().push_back((*animVec)[i]);
								animNames[aModel->ModelName()].push_back((*animVec)[i]->GetName());
								break;
							}
						}
						for (int i = 0; i < animNames[aModel->ModelName()].size(); i++)
						{
							if (animNames[aModel->ModelName()][i] == data->GetName())
							{
								animNames[aModel->ModelName()].pop_back();
								break;
							}
						}
						animNames[aModel->ModelName()].push_back(data->GetName());
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Separator();
				ImGui::Separator();

				if (GetClipIndex() > -1)
				{

					ImGui::BulletText("Frame : %d", aModel->GetAnimClip()[GetClipIndex()]->CurrentFrame());
					ImGui::SliderFloat("Time", &aModel->GetAnimClip()[GetClipIndex()]->StartTime(), 0, aModel->GetAnimClip()[GetClipIndex()]->Duration(), "%.0f");

					ImGui::SliderFloat("Speed", &aModel->GetAnimClip()[GetClipIndex()]->GetSpeed(), 0, 30);
					speed = aModel->GetAnimClip()[GetClipIndex()]->GetSpeed();
					ImGui::InputFloat("Start(ms)", &startTime);
					ImGui::InputFloat("Blend", &blendTime);
					ImGui::Checkbox("Repeat", &isRepeat);
					if (ImGui::Button("Play"))
					{
						if (GetClipIndex() > -1)
							aModel->Play(GetClipIndex(), isRepeat, blendTime, speed, startTime);
					}
					ImGui::SameLine();
					if (ImGui::Button("Stop"))
					{
						aModel->Stop();
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset"))
					{
						if (GetClipIndex() > -1)
							aModel->Reset(GetClipIndex());

						aModel->GetTweener()->IsPlay() = false;
					}
				}
			}

			//////////////////////본 임구이랜더쪽///////////////////////
			bonesIter miter1;
			if (modelType == L"DefaultAnim")
				miter1 = bonesMap.find(aModel->ModelName());

			for (int i = 0; i < miter1->second.size(); i++)
				miter1->second[i]->Render();

			for (int i = 0; i < miter1->second.size(); i++)
			{
				int selectIdx = -1;
				if (miter1->second[i]->ClickState())
				{
					selectIdx = i;
					
					for (int j = 0; j < miter1->second.size(); j++)
						miter1->second[j]->PickState() = false;
					
					miter1->second[selectIdx]->PickState() = true;
				}

				if (miter1->second[i]->PickState())
				{
					if (Mouse::Get()->Down(1))
						miter1->second[i]->PickState() = false;

					ImGui::Checkbox("Visible", &(miter1->second[i]->isVisible));
					ImGui::SameLine();
					ImGui::Checkbox("Active", &(miter1->second[i]->isActive));
					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "=======[%s]=======", String::ToString(miter1->second[i]->ModelName()).c_str());

					ImGui::Separator();
					ImGui::Separator();
					ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[Center Position]");
					ImGui::Text("Center(%.2f,%.2f,%.2f)", miter1->second[i]->GetCenterPos().x, miter1->second[i]->GetCenterPos().y, miter1->second[i]->GetCenterPos().z);

					ImGui::Separator();
					ImGui::Separator();
					wstring textTrans = L"[" + miter1->second[i]->ModelName() + L"]translate";
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
					ImGui::Text(String::ToString(textTrans).c_str());
					ImGui::DragFloat3((String::ToString(miter1->second[i]->ModelName()) + "T").c_str(), (float*)&miter1->second[i]->BonePosition(), 0.05f, -128, 128);
					miter1->second[i]->UpdateOutMatrix();

					wstring textScale = L"[" + miter1->second[i]->ModelName() + L"]scale";
					ImGui::Text(String::ToString(textScale).c_str());
					ImGui::DragFloat3((String::ToString(miter1->second[i]->ModelName()) + "S").c_str(), (float*)&miter1->second[i]->BoneScale(), 0.05f, -10, 10);
					miter1->second[i]->UpdateOutMatrix();

					wstring textRotate = L"[" + miter1->second[i]->ModelName() + L"]rotate";
					ImGui::Text(String::ToString(textRotate).c_str());
					ImGui::DragFloat3((String::ToString(miter1->second[i]->ModelName()) + "R").c_str(), (float*)&miter1->second[i]->BoneRotate(), 0.05f, -3.14f, 3.14f);
					miter1->second[i]->UpdateOutMatrix();
				}
			}
		}
		break;
	}
}

void CharactorTool::InputModelPtr(GameModel * model)
{
	modelType = model->GetModelType();
	if (model->GetModelType() == L"DefaultAnim")
	{
		aModel = dynamic_cast<GameAnimModel*>(model);

		Shader*shader = new Shader(Shaders + L"010_Sphere.hlsl");

		if (bonesMap[aModel->ModelName()].size() > 0)return;
		for (int i = 0; i < aModel->GetModel()->Bones().size(); i++)
			InputBoneData(shader, aModel, aModel->ModelName(), i);
	}
	else
	{
		nModel =model;
	}
}



void CharactorTool::SetAnimVec(vector<class ModelClip*>& animVec)
{
	this->animVec = &animVec;
	int a = 10;
}

void CharactorTool::InputBoneData(class Shader*shader, class GameModel*model, wstring modelName, int idx)
{
	BoneSphere*boneSphere = new BoneSphere(values);
	boneSphere->SetShader(shader);
	boneSphere->ModelName() = model->GetModel()->BoneByIndex(idx)->Name();
	//구의 위치를 본의 위치로 바꿔야한다.
	//본의 메트릭스에서 위치값만 빼온다.

	D3DXMATRIX boneMatrix = model->GetModel()->BoneByIndex(idx)->Global(); // 이미월드로 변환된뼈행렬
	D3DXMATRIX modelScaleMat = model->GetModel()->Buffer()->ScaleMatrix();
	D3DXVECTOR3 modelScale = model->GetModel()->Buffer()->Scale();
	D3DXVECTOR3 pos(0, 0, 0);

	D3DXMATRIX result = modelScaleMat*boneMatrix;
	D3DXVec3TransformCoord(&pos, &pos, &result);

	if (model->GetModelType() == L"DefaultAnim")
	{
		D3DXVECTOR3 modelScale = model->Scale();
		
		boneSphere->Scale(3, 3, 3);
		boneSphere->Position(pos.x, pos.y, pos.z);
		boneSphere->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
	}

	bonesMap[modelName].push_back(boneSphere);
	bonesScaleMap[modelName].push_back(boneSphere->Scale());
}

void CharactorTool::UpdateBoneData()
{
}

int CharactorTool::GetClipIndex()
{
	if (modelType == L"DefaultAnim")
	{
		for (int i = 0; i < animNames[aModel->ModelName()].size(); i++)
		{
			if (animNames[aModel->ModelName()][i] == selectAnimName)
			{
				return i;
			}
		}
		return -1;
	}
	else
	{
		for (int i = 0; i < animNames[nModel->ModelName()].size(); i++)
		{
			if (animNames[nModel->ModelName()][i] == selectAnimName)
			{
				return i;
			}
		}
		return -1;
	}
}

void CharactorTool::LoadShaderFile(wstring filename)
{
	if (modelType == L"DefaultAnim")
	{
		Shader * shader = new Shader(filename);
		aModel->SetShader(shader);
		wstring temp = filename;
		shaderName = Path::GetFileName(temp);
	}
	else
	{
		Shader * shader = new Shader(filename);
		nModel->SetShader(shader);
		wstring temp = filename;
		shaderName = Path::GetFileName(temp);
	}
}

void CharactorTool::DrawBoneNameText()
{
	bonesIter biter = bonesMap.begin();

	for (; biter != bonesMap.end(); biter++)
	{
		for (int i = 0; i < biter->second.size(); i++)
		{
			if (!biter->second[i]->PickState())continue;
			
			//월드 뷰 프로젝션 이차원좌표를 구해야함
			D3DXMATRIX V, P;
			values->MainCamera->Matrix(&V);
			values->Perspective->GetMatrix(&P);

			D3DXMATRIX matrix;
			D3DXMatrixMultiply(&matrix, &V, &P);
			D3DXMATRIX WVP = biter->second[i]->World()*matrix;

			D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
			D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);
			
			//NDC 공간으로 변환
			float wScreenX = worldToScreen.x / worldToScreen.w;
			float wScreenY = worldToScreen.y / worldToScreen.w;
			float wScreenZ = worldToScreen.z / worldToScreen.w;

			//-1~1 구간을 0~1구간으로 변환
			float nScreenX = (wScreenX + 1)*0.5f;
			float nScreenY = 1.0f-(wScreenY + 1)*0.5f;
			
			//최종화면의 좌표
			float resultX = nScreenX*values->Viewport->GetWidth();
			float resultY = nScreenY*values->Viewport->GetHeight();

			RECT rect = { resultX,resultY,resultX + 100,resultY + 100 };
			
			DirectWrite::Get()->RenderText(biter->second[i]->ModelName(), rect, 15, L"돋음체",D3DXCOLOR(1,0,0,1));
		}
	}
}

void CharactorTool::InputAnimNames(wstring modelName, wstring animName)
{
	animNames[modelName].push_back(animName);
}
