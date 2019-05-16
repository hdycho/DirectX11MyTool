#include "Framework.h"
#include "ColliderManager.h"


ColliderManager*ColliderManager::instance = NULL;

void ColliderManager::Create()
{
	CollisionContext::Create();

	if (instance == NULL)
		instance = new ColliderManager();

}

void ColliderManager::Delete()
{
	SAFE_DELETE(instance);

	CollisionContext::Delete();
}

ColliderManager::ColliderManager()
{
	CollisionContext::Get()->AddLayer(L"Collision Weapon");
	CollisionContext::Get()->AddLayer(L"Collision Bullet");
	CollisionContext::Get()->AddLayer(L"Collision Item");
	CollisionContext::Get()->AddLayer(L"Collision Player");
	CollisionContext::Get()->AddLayer(L"Collision PlayerSkill");
	CollisionContext::Get()->AddLayer(L"Collision Enemy");
	CollisionContext::Get()->AddLayer(L"Collision EnemyAttack");
	CollisionContext::Get()->AddLayer(L"Collision EnemySkill1");
	CollisionContext::Get()->AddLayer(L"Collision EnemySkill2");
	CollisionContext::Get()->AddLayer(L"Collision Character");
	CollisionContext::Get()->AddLayer(L"Collision World");

	layerNames.push_back(L"Collision Weapon");
	layerNames.push_back(L"Collision Bullet");
	layerNames.push_back(L"Collision Item");
	layerNames.push_back(L"Collision Player");
	layerNames.push_back(L"Collision PlayerSkill");
	layerNames.push_back(L"Collision Enemy");
	layerNames.push_back(L"Collision EnemyAttack");
	layerNames.push_back(L"Collision EnemySkill1");
	layerNames.push_back(L"Collision EnemySkill2");
	layerNames.push_back(L"Collision Character");
	layerNames.push_back(L"Collision World");

	selectLayerName = L"";
	inputColliderName = L"";

}

ColliderManager::~ColliderManager()
{
	colliderVec.clear();
	layerNames.clear();
}

void ColliderManager::CreateLayer(wstring layerName)
{
	CollisionContext::Get()->AddLayer(layerName);
}

void ColliderManager::AddLayerToCollider(ColliderElement*collider)
{
	CollisionContext::Get()->GetLayer(selectLayerName)->AddCollider(collider);
}

void ColliderManager::AddLayerToCollider(wstring layerName, ColliderElement * collider)
{
	CollisionContext::Get()->GetLayer(layerName)->AddCollider(collider);
}

void ColliderManager::ImGuiLayerMenu()
{
	ImGui::Begin("ColliderManager");
	if (ImGui::BeginCombo("Layers", String::ToString(selectLayerName).c_str()))
	{
		for (size_t i = 0; i < layerNames.size(); i++)
		{
			bool isSelected = (selectLayerName == layerNames[i]);
			if (ImGui::Selectable(String::ToString(layerNames[i]).c_str(), isSelected))
			{
				selectLayerName = layerNames[i];

			}
			if (isSelected == true)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	if (ImGui::BeginCombo("InputCollider", String::ToString(inputColliderName).c_str()))
	{
		ImGui::EndCombo();
	}
	else if (ImGui::BeginDragDropTarget())//머터리얼 받을 타겟
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Collider"))
		{
			IM_ASSERT(payload->DataSize == sizeof(ColliderElement*));
			ColliderElement**data = (ColliderElement**)payload->Data;

			inputColliderName = (*data)->Name();
			AddLayerToCollider(*data);
		}
		ImGui::EndDragDropTarget();
	}

	/*if (ImGui::Button("Input"))
	{
	if (inputColliderName != L"")
	{
	AddLayerToCollider(*data);
	inputColliderName
	}
	}*/

	ImGui::End();
}

void ColliderManager::InputObject(ICollider * obj)
{
	colliderVec.push_back(obj);
}

void ColliderManager::Erase(ICollider * obj)
{
	vIter viter = colliderVec.begin();

	for (; viter != colliderVec.end(); viter++)
	{
		if ((*viter) == obj)
		{
			colliderVec.erase(viter);
			break;
		}
	}

}

void ColliderManager::Update()
{
	for (ICollider*collider : colliderVec)
	{
		collider->ColliderUpdate();
	}
}

void ColliderManager::DeleteAll()
{
	CollisionContext::Get()->DeleteAll();
	colliderVec.clear();
}
