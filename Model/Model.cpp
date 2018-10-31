#include "stdafx.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"

Model::Model()
{
	buffer = new BoneBuffer();
}

Model::Model(Model & model)
{
	buffer = new BoneBuffer();
	memcpy(buffer->Data.Bones, model.buffer->Data.Bones,sizeof(D3DXMATRIX)*128);

	for (int i = 0; i < model.materials.size(); i++)
	{
		Material*copyMat = NULL;
		model.materials[i]->Clone((void**)&copyMat);
		materials.push_back(copyMat);
	}
	
	for (int i = 0; i < model.meshes.size(); i++)
	{
		ModelMesh*copyMesh = NULL;
		model.meshes[i]->Clone((void**)&copyMesh);
		meshes.push_back(copyMesh);
	}
	
	for (int i = 0; i < model.bones.size(); i++)
	{
		ModelBone*copyBone = NULL;
		model.bones[i]->Clone((void**)&copyBone);
		bones.push_back(copyBone);
	}

	BindingBone();
	BindingMesh();
}

Model::~Model()
{
	SAFE_DELETE(buffer);

	for (Material*material : materials)
		SAFE_DELETE(material);

	for (ModelBone*bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh*mesh : meshes)
		SAFE_DELETE(mesh);
}

Material * Model::MaterialByName(wstring name)
{
	for (Material*material : materials)
	{
		if (material->Name() == name)
			return material;
	}

	return NULL;
}

ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh*mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}
	
	return NULL;
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone*bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}
	
	return NULL;
}

void Model::CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms)
{
	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);

	CopyGlobalBoneTo(transforms, w);
}

void Model::CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms, D3DXMATRIX& w)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone*bone = bones[i];

		if (bone->Parent() != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->local*transforms[index];
		}
		else
		{
			transforms[i] = bone->local*w;
		}
	}
}

void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material*>>temp : materialMap)
	{
		for (Material*material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}
}
