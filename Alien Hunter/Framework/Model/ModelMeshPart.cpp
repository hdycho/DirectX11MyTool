#include "Framework.h"
#include "ModelMeshPart.h"

ModelMeshPart::ModelMeshPart()
	: pass(0), boneIndexVariable(NULL)
{

}

ModelMeshPart::~ModelMeshPart()
{
}

void ModelMeshPart::Render()
{
	if (boneIndexVariable == NULL)
		boneIndexVariable = material->GetEffect()->AsScalar("BoneIndex");

	boneIndexVariable->SetInt(parent->ParentBoneIndex());
	material->GetEffect()->Draw(0, pass, vertexCount, startVertex);
}

void ModelMeshPart::RenderInstance(UINT count, UINT tec)
{
	if (boneIndexVariable == NULL)
		boneIndexVariable = material->GetEffect()->AsScalar("BoneIndex");

	boneIndexVariable->SetInt(parent->ParentBoneIndex());
	material->GetEffect()->DrawInstanced(tec, pass, vertexCount, count, startVertex);
}

void ModelMeshPart::Release()
{
}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart *part = new ModelMeshPart();

	part->materialName = this->materialName;
	part->startVertex = this->startVertex;
	part->vertexCount = this->vertexCount;

	*clone = part;
}