#include "stdafx.h"
#include "CameraManager.h"
#include "../Objects/GameModel.h"
#include "../Viewer/Frustum.h"
#include "../Objects/Player.h"

CameraManager*CameraManager::instance = NULL;

CameraManager * CameraManager::GetInstance()
{
	if (instance == NULL)
		instance = new CameraManager;

	return instance;
}

void CameraManager::Init(ExecuteValues*values)
{
	//게임에서 실제보이는 카메라는 하나라고 가정하고 만든다
	//나중에 여러개의 랜더타겟이나 미니맵만들때 수정하기
	
	modelName = L"";

	Camera*freedom = new Freedom(80);
	cams.push_back(freedom);

	Camera*fixity = new Fixity();
	cams.push_back(fixity);

	Camera*orbit = new Orbit();
	cams.push_back(orbit);

	Camera*follow = new Follow();
	cams.push_back(follow);

	Camera*tps = new TPSCamera(20,0.5f);
	cams.push_back(tps);

	this->values = values;

	this->values->MainCamera = cams[(int)CAMERASTATE::Freedom];
	this->values->MainCamera->RotationDegree(10, 0);
	this->values->MainCamera->Position(30.13f, 177, -339.5f);
	
	stateNames.push_back(L"Freedom");
	stateNames.push_back(L"Fixity");
	stateNames.push_back(L"Orbit");
	stateNames.push_back(L"Follow");
	stateNames.push_back(L"TPSCamera");

	selectStateName = L"Freedom";
	cState = CAMERASTATE::Freedom;
}

void CameraManager::Update()
{
	if (selectStateName == L"Freedom")
		cState = CAMERASTATE::Freedom;
	else if (selectStateName == L"Fixity")
		cState = CAMERASTATE::Fixity;
	else if (selectStateName == L"Orbit")
		cState = CAMERASTATE::Orbit;
	else if (selectStateName == L"Follow")
		cState = CAMERASTATE::Follow;
	else if (selectStateName == L"TPSCamera")
		cState = CAMERASTATE::TPS;
}

void CameraManager::Render()
{
}

void CameraManager::ImGuiRender()
{
	ImGui::Begin("CameraManager");


	if (ImGui::BeginCombo("Tag", String::ToString(selectStateName).c_str()))
	{
		for (int i = 0; i < stateNames.size(); i++)
		{
			bool isSelected = (selectStateName == stateNames[i]);
			if (ImGui::Selectable(String::ToString(stateNames[i]).c_str(), isSelected))
			{
				selectStateName = stateNames[i];
			}
			if (isSelected == true)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	switch (cState)
	{
		case CameraManager::CAMERASTATE::Freedom:
		{
		}
		break;
		case CameraManager::CAMERASTATE::Fixity:
		{
			
		}
		break;
		case CameraManager::CAMERASTATE::Orbit:
		{
			if (ImGui::BeginCombo("Target", String::ToString(modelName).c_str()))
			{
				ImGui::EndCombo();
			}
			else if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjPayload"))
				{
					GameModel*p = (GameModel*)payload->Data;
					IM_ASSERT(payload->DataSize == sizeof(GameModel));
					model = p->GetPtr();
					OstartPos = model->Position();
					modelName = model->ModelName();
				}
				ImGui::EndDragDropTarget();
			}
		}
		break;
		case CameraManager::CAMERASTATE::Follow:
		{
			if (ImGui::BeginCombo("Target", String::ToString(modelName).c_str()))
			{
				ImGui::EndCombo();
			}
			else if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjPayload"))
				{
					GameModel*p = (GameModel*)payload->Data;
					IM_ASSERT(payload->DataSize == sizeof(GameModel));
					model = p->GetPtr();
					FstartPos = &model->Position();
					Frotate = &model->Rotation();
					modelName = model->ModelName();
				}
				ImGui::EndDragDropTarget();
			}
		}
		break;
		case CameraManager::CAMERASTATE::TPS:
		{
			if (ImGui::BeginCombo("Target", String::ToString(modelName).c_str()))
			{
				ImGui::EndCombo();
			}
			else if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjPayload"))
				{
					GameModel*p = (GameModel*)payload->Data;
					IM_ASSERT(payload->DataSize == sizeof(GameModel));
					model = p->GetPtr();
					TpsPos = &model->Position();
					TpsRotate = &model->Rotation();
					modelName = model->ModelName();
				}
				ImGui::EndDragDropTarget();
			}
		}
		break;
	}
	ImGui::SameLine();
	if (ImGui::Button("Select"))
		SelectCamera(cState);

	switch (cState)
	{
		case CameraManager::CAMERASTATE::Freedom:
		{
			ImGui::TextColored(ImVec4(0.8, 0, 0.8, 1), "[FrustumInfo]");
			frustum->ImguiRender();
		}
		break;
		case CameraManager::CAMERASTATE::Fixity:
		{
			if (values->MainCamera == cams[(int)CAMERASTATE::Fixity])
			{
				Fixity*downCast = dynamic_cast<Fixity*>(values->MainCamera);

				ImGui::Text("SetTargetPosition");
				ImGui::DragFloat3("TargetPos", downCast->Position(), 1);
				ImGui::Text("SetRotate");
				ImGui::DragFloat2("Rotation", downCast->GetRotation(), 0.1f);
			}
		}
		break;
		case CameraManager::CAMERASTATE::Orbit:
		{
			if (values->MainCamera == cams[(int)CAMERASTATE::Orbit])
			{
				Orbit*downCast = dynamic_cast<Orbit*>(values->MainCamera);

				ImGui::Text("SetTargetPosition");
				ImGui::DragFloat3("TargetPos", downCast->GetTargetPos(), 1);
				ImGui::Text("SetDistance");
				ImGui::DragFloat("Distance", &downCast->GetSetZ(), 0.5f);
				ImGui::Text("SetRotate");
				ImGui::DragFloat2("Rotation", &downCast->GetRotate().x, 0.1f);
			}
		}
		break;
		case CameraManager::CAMERASTATE::Follow:
		{
			if (values->MainCamera == cams[(int)CAMERASTATE::Follow])
			{
				Follow*downCast = dynamic_cast<Follow*>(values->MainCamera);

				ImGui::Text("SetHeight");
				ImGui::DragFloat("Height", &downCast->GetSetY(), 0.5f);
				ImGui::Text("SetDistance");
				ImGui::DragFloat("Distance", &downCast->GetSetZ(), 0.5f);
				ImGui::Text("SetRotate");
				ImGui::DragFloat("Rotation", &downCast->GetRotate().x, 0.1f);
			}
		}
		break;
		case CameraManager::CAMERASTATE::TPS:
		{
			if (values->MainCamera == cams[(int)CAMERASTATE::TPS])
			{
				TPSCamera*downCast = dynamic_cast<TPSCamera*>(values->MainCamera);

				ImGui::Text("SetHeight");
				ImGui::DragFloat("Height", &downCast->GetSetY(), 0.5f);
				ImGui::Text("SetDistance");
				ImGui::DragFloat("Distance", &downCast->GetSetZ(), 0.5f);
				ImGui::Text("SetRotate");
				ImGui::DragFloat("Rotation", &downCast->GetRotate().x, 0.1f);
			}
		}
		break;
	}

	ImGui::End();
}

void CameraManager::SelectCamera(CAMERASTATE cState)
{
	switch (cState)
	{
		case CameraManager::CAMERASTATE::Freedom:
		{
			
			values->MainCamera = cams[(int)CAMERASTATE::Freedom];
		}
		break;
		case CameraManager::CAMERASTATE::Fixity:
		{

			values->MainCamera = cams[(int)CAMERASTATE::Fixity];
			values->MainCamera->Position(0, 50, 0);
			values->MainCamera->Rotation(Math::ToRadian(-90), 0);
		}
		break;
		case CameraManager::CAMERASTATE::Orbit:
		{

			values->MainCamera = cams[(int)CAMERASTATE::Orbit];
			Orbit*downCast = dynamic_cast<Orbit*>(values->MainCamera);
			downCast->InputInfo(OstartPos);
		}
		break;
		case CameraManager::CAMERASTATE::Follow:
		{
		
			values->MainCamera = cams[(int)CAMERASTATE::Follow];
			
			Follow*downCast = dynamic_cast<Follow*>(values->MainCamera);
			downCast->InputInfo(FstartPos, Frotate);
		}
		break;
		case CameraManager::CAMERASTATE::TPS:
		{
			
			values->MainCamera = cams[(int)CAMERASTATE::TPS];

			TPSCamera*downCast = dynamic_cast<TPSCamera*>(values->MainCamera);
			downCast->InputInfo(TpsPos, TpsRotate);
			
			Player*downPlayer = dynamic_cast<Player*>(model);
			downPlayer->SetCamera(downCast);
		}
		break;
	}
}
