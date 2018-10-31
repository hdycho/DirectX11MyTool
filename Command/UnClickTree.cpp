#include "stdafx.h"
#include "./Objects/TreesManager.h"
#include "./Objects/Billboard.h"
#include "UnClickTree.h"

void UnClickTree::Execute(TreesManager * tm, void * data)
{
	ctype = UNCLICK;
	TreesManager::bIter biter = tm->GetTreeMap().begin();

	for (; biter != tm->GetTreeMap().end(); biter++)
	{
		wstring name = Path::GetFileName(tm->GetTreeName());
		if (name == biter->first)
		{
			biter->second[tm->GetTreesIdx().top()]->GetCollider()->PickingState() = false;
			tm->GetTreesIdx().pop();
			break;
		}
	}
}
