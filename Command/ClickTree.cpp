#include "stdafx.h"
#include "ClickTree.h"
#include "./Objects/TreesManager.h"
#include "./Objects/Billboard.h"

void ClickTree::Execute(TreesManager * tm, void * data)
{
	ctype = CLICK;
	TreesManager::bIter biter = tm->GetTreeMap().begin();

	for (; biter != tm->GetTreeMap().end(); biter++)
	{
		for (int i = 0; i < biter->second.size(); i++)
		{
			D3DXMATRIX modelWorld;
			modelWorld = biter->second[i]->World();
			if (biter->second[i]->GetCollider()->IsPicked(tm->GetValues(), modelWorld))
			{
				if (!biter->second[i]->GetCollider()->PickingState())
				{
					tm->GetTreesIdx().push(i);
					biter->second[i]->GetCollider()->PickingState() = true;
					return;
				}
				else
				{
					tm->GetTreesIdx().pop();
					biter->second[i]->GetCollider()->PickingState() = false;
				}
			}
		}
	}
}
