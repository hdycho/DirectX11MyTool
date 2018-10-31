#include "stdafx.h"
#include "ModelBone.h"

ModelBone::ModelBone()
	:parent(NULL)
{
	isLocalChange = false;
}

ModelBone::~ModelBone()
{
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();
	bone->index = index;
	bone->name = name;
	bone->parentIndex = parentIndex;
	bone->local = local;
	bone->global = global;
	isLocalChange = false;
	*clone = bone;
}
