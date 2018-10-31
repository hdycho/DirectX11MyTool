#include "stdafx.h"
#include "CollisionManager.h"
#include "../Physics/Collider.h"
#include "../Interfaces/ICollision.h"
#include "../Objects/GameModel.h"

CollisionManager* CollisionManager::instance = NULL;

bool CollisionManager::IsCollision(COLLIDERSTATE cstate,vector<ColliderInfo>& model1, vector<ColliderInfo>& model2)
{
	if (cstate == COLLIDERSTATE::BOX)
	{
		//0���� �ּ� 1���� �ִ������� �Ǵ�
		//1���� �����浹
		if (model1[1].pos.x > model2[0].pos.x&&model1[0].pos.x < model2[1].pos.x&&
			model1[0].pos.z<model2[1].pos.z&&model1[1].pos.z>model2[0].pos.z&&
			model1[0].pos.y<model2[1].pos.y&&model1[1].pos.y>model2[0].pos.y)
		{
			return true;
		}
		else
			return false;
	}
	else if (cstate == COLLIDERSTATE::CAPSULE)
	{
		float s, t;
		D3DXVECTOR3 c1, c2;

		D3DXVECTOR3 a1, b1, a2, b2;
		a1 = model1[0].heightPos[0];
		b1 = model1[0].heightPos[1];
		a2 = model2[0].heightPos[0];
		b2 = model2[0].heightPos[1];

		float dist2 = Math::ClosestPtSegmentSegment(a1, b1, a2, b2, s, t, c1, c2);
		float radius = model1[0].radius + model2[0].radius;

		bool result = dist2 <= (radius * radius);

		return result;
	}

	return false;
}

CollisionManager*CollisionManager::GetInstance()
{
	if (instance == NULL)
	{
		instance = new CollisionManager();
	}
	return instance;
}

void CollisionManager::Update()
{
	//�浹ó���� ��� ������Ʈ�� �ݶ��̴� 0���� �ٸ� ��ü�� 0���� �ƴѰ͵�� 
	//�浹�� �����̵ȴ�.

	for (int i = 0; i < model.size(); i++)
	{
		for (int j = 0; j < model.size(); j++)
		{
			if (i == j)continue;
			if (model[j]->GetColliders().size() < 1)continue;
			for (int k = 1; k < model[j]->GetColliders().size(); k++)
			{
				if (IsCollision(
					model[i]->GetColliders()[0]->ColliderState(),
					model[i]->GetColliders()[0]->InfoCollider(),
					model[j]->GetColliders()[k]->InfoCollider()))
				{
					model[i]->OnCollisionEnter();
				}
				else
				{
					model[i]->OnCollisionExit();
				}
			}
		}
	}

	//for (int i = 0; i < model.size(); i++)
	//{
	//	for (int j = 0; j < model.size()-(i+1); j++)
	//	{
	//		if (IsCollision(
	//			model[j]->GetColliders()[0]->ColliderState(),
	//			model[j]->GetColliders()[0]->InfoCollider(),
	//			model[j+1]->GetColliders()[0]->InfoCollider()))
	//		{
	//			model[j]->OnCollisionEnter();
	//		}
	//		else
	//		{
	//			model[j]->OnCollisionExit();
	//		}
	//	}
	//}
}


void CollisionManager::Render()
{
	ImGui::Begin("CollisionManager");
	{
		ImGui::Text("Collision Target");
		ImGui::InputInt("Target Count", &sizeNum);

		if (sizeNum != saveNum)
		{
			//TODO:ũ��ø��� �����������ؼ� �ֱ�
			model.resize(sizeNum);
			modelNames.assign(sizeNum, L"");
			saveNum = sizeNum;
		}

		for (int i = 0; i < model.size(); i++)
		{
			if (ImGui::BeginCombo("CollsionObj",String::ToString(modelNames[i]).c_str()))
			{
				ImGui::EndCombo();
			}
			else if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjPayload"))
				{
				/*	GameModel*p = (GameModel*)payload->Data;
					if (p->ClassName() == L"Player")
					{
						IM_ASSERT(payload->DataSize == sizeof(GameModel));
						model[i] = p->GetPtr();
						modelNames[i]= model[i]->ModelName();
					}*/

					GameModel*p = (GameModel*)payload->Data;
					IM_ASSERT(payload->DataSize == sizeof(GameModel));
					model[i] = p->GetPtr();
					modelNames[i] = model[i]->ModelName();
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
	ImGui::End();
}