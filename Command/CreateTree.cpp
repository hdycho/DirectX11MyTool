#include "stdafx.h"
#include "CreateTree.h"
#include "./Objects/TreesManager.h"
#include "./Objects/Billboard.h"

void CreateTree::Execute(class TreesManager*tm, void * data)
{
	if (data != NULL)
	{
		Data = data;
		ctype = CREATE;
		pos = *(D3DXVECTOR3*)Data;
		//tm->GetTreesPos().push(pos);
	}
	tm->GetTreesPos().push(pos);
	switch (tm->GetType())
	{
		case TreesManager::SINGLE:
		{
			Billboard*billboard = new Billboard(tm->GetValues(), Shaders + L"016_Billboard.hlsl", tm->GetTreeName());

			billboard->Position(pos.x, pos.y+10, pos.z);
			billboard->Scale(15, 15, 5);
			billboard->RenewCollider();

			wstring name = Path::GetFileName(tm->GetTreeName());
			tm->GetTreeMap()[name].push_back(billboard);
		}
		break;
		case TreesManager::MULTI:
		{
			Billboard*billboard1 = new Billboard(tm->GetValues(), Shaders + L"016_Billboard.hlsl", tm->GetTreeName());

			billboard1->Position(pos.x, pos.y, pos.z);
			billboard1->Scale(15, 15, 5);
			billboard1->SetAngle(45);
			billboard1->RenewCollider();

			wstring name = Path::GetFileName(tm->GetTreeName());
			tm->GetTreeMap()[name].push_back(billboard1);

			Billboard*billboard2 = new Billboard(tm->GetValues(), Shaders + L"016_Billboard.hlsl", tm->GetTreeName());

			billboard2->Position(pos.x, pos.y, pos.z);
			billboard2->Scale(15, 15, 5);
			billboard2->SetAngle(-45);
			billboard2->RenewCollider();

			tm->GetTreeMap()[name].push_back(billboard2);
		}
		break;
	}
}


