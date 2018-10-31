#include "stdafx.h"
#include "RemoveTree.h"
#include "./Objects/TreesManager.h"
#include "./Objects/Billboard.h"

void RemoveTree::Execute(TreesManager * tm, void * data)
{
	ctype= REMOVE;
	/*switch (tm->GetType())
	{*/
	TreesManager::bIter biter = tm->GetTreeMap().begin();

	for (; biter != tm->GetTreeMap().end(); biter++)
	{
		TreesManager::bvIter bviter = biter->second.begin();
		
		if(biter->second.size()>0)
			biter->second.erase(bviter +biter->second.size()-1);
		return;
	}
	//}
}
