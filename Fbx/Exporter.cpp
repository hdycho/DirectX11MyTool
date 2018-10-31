#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "Utility.h"
#include "../Utilities/Xml.h"
#include "../Utilities/BinaryFile.h"

Fbx::Exporter::Exporter(wstring file)
{
	//   �Ŵ��� ����
	manager = FbxManager::Create();
	//   �� ����(Ʈ�� ���·� ������ ��带 ������ ����)
	scene = FbxScene::Create(manager, "");

	ios = FbxIOSettings::Create(manager, IOSROOT);
	//   FBX ���� �ɼ� ����
	//   FBX ���� ���ϴ� ������ ������ ���� Ǯ�� �������·� ������
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	//   �Ŵ����� ios ����
	manager->SetIOSettings(ios);

	//   ������ ����
	//   �����Ϳ� �Ŵ��� ����
	//   "" : �̸�
	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(file);
	//   (���� �̸�, ���˹�ȣ, IOS)
	//   ���� ��ȣ : -1 - ��� ����
	//            0 - FBX
	//            1 - OBJ
	//   ...

	//   importer�� ���� �ʱ�ȭ
	//   import��ų ����, ���� ����, IOS(���ٸ�, �ڵ��� ����Ʈ ios�� ����)
	bool b = importer->Initialize(sFile.c_str(), -1, ios);
	assert(b == true);

	//   �� ������ �����ٰ� ����
	//   �� �ؿ� ��Ʈ ��尡 ����
	b = importer->Import(scene);
	assert(b == true);

	//   Mesh, Nurbs, Patches�� �����ϰ�
	//   �ﰢ��ȭ ��Ű��, Nurbs�� Patches�� ������ �����ϰ� ���ִ� Ŭ����
	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);

	importer->Destroy();
	ios->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring fileName)
{
	ReadMaterial();
	WriteMaterial(saveFolder, fileName);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	WriteMesh(saveFolder, fileName);
}


void Fbx::Exporter::GetClipList(vector<wstring>* list)
{
	list->clear();
	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo *takeInfo = importer->GetTakeInfo(i);

		string name = takeInfo->mName.Buffer();
		list->push_back(String::ToWString(name));
	}
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring fileName, UINT clipNumber)
{
	FbxClip * clip = ReadAnimationData(clipNumber);
	WriteClipData(clip, saveFolder, fileName);

}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring fileName, wstring clipName)
{
	FbxClip * clip = ReadAnimationData(clipName);
	WriteClipData(clip, saveFolder, fileName);
}


void Fbx::Exporter::ReadMaterial()
{
	//   ������ ��Ƽ������ ������ �޾ƿ�
	//   �� Fbx�� ���͸����� ������ �Ѱ� �̻��̴�.
	int count = scene->GetMaterialCount();

	for (int i = 0; i < count; i++)
	{
		//   ������ ������ ���͸��� ������ �޾ƿ�
		FbxSurfaceMaterial * fbxMaterial = scene->GetMaterial(i);
		//   XML �������� �����ϱ� ����
		//   ����ü�� ����
		FbxMaterial * material = new FbxMaterial;
		material->Name = fbxMaterial->GetName();

		//   FbxSurfaceMaterial�� FbxSurfaceLambert�� �ٿ�ĳ������ ��������
		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			// �����ϴٸ� �ٿ�ĳ������ lambert������ �޾ƿ�
			FbxSurfaceLambert * lambert = (FbxSurfaceLambert*)fbxMaterial;

			//   lambert->Diffuse : DiffuseĮ�� ��(rgb)
			//   lambert->DiffuseFactor : ����(a)
			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
		}

		//   ���� ����
		FbxProperty prop;
		//   ���� Diffuse�� �̸����� ������ ã�Ƽ� �޾ƿ�
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		//   Diffuse�� ���ϰ�θ� ����
		material->DiffuseFile = Utility::GetTextureFile(prop);

		//	���� Specular�� �̸����� ���� ã�Ƽ� �޾ƿ�
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = Utility::GetTextureFile(prop);

		//	���� Specular�� �̸����� ���� ã�Ƽ� �޾ƿ�
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		material->NormalMapFile = Utility::GetTextureFile(prop);

		materials.push_back(material);
	}
}

void Fbx::Exporter::WriteMaterial(wstring saveFolder, wstring fileName)
{
	//   ������ ��ġ�� ������ ����
	Path::CreateFolder(saveFolder);

	//   ��� ���� XMLDocument�� ����ȴ�
	//   document�� ������� ����� ��� ��嵵 ���� �������.
	Xml::XMLDocument * document = new Xml::XMLDocument();
	//   Declaration(����)�� ù �������� �ȴ�.
	Xml::XMLDeclaration * decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	//   Materials�� ��带 root ���� �Ʒ���
	//   Material���� ���� ����
	Xml::XMLElement * root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (FbxMaterial* material : materials)
	{
		//   root �Ʒ��� ����
		Xml::XMLElement * node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement * element = NULL;

		//   Material�̶� ��� �Ʒ��� ����
		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		node->LinkEndChild(element);

		//   Diffuse��� �Ʒ��� �� ����
		WriteXmlColor(element, document, material->Diffuse);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		CopyTextureFile(material->SpecularFile, saveFolder);
		element->SetText(material->SpecularFile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalMap");
		CopyTextureFile(material->NormalMapFile, saveFolder);
		element->SetText(material->NormalMapFile.c_str());
		node->LinkEndChild(element);

		//   ReadMaterial���� �Ҵ��� material ����
		SAFE_DELETE(material);
	}

	//   ������ ���� ��ο� ����
	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str());
}

void Fbx::Exporter::WriteXmlColor(Xml::XMLElement * element, Xml::XMLDocument * document, D3DXCOLOR color)
{
	Xml::XMLElement * newColor = NULL;

	newColor = document->NewElement("R");
	newColor->SetText(color.r);
	element->LinkEndChild(newColor);

	newColor = document->NewElement("G");
	newColor->SetText(color.g);
	element->LinkEndChild(newColor);

	newColor = document->NewElement("B");
	newColor->SetText(color.b);
	element->LinkEndChild(newColor);

	newColor = document->NewElement("A");
	newColor->SetText(color.a);
	element->LinkEndChild(newColor);
}

void Fbx::Exporter::CopyTextureFile(OUT string & textureFile, wstring & saveFolder)
{
	if (textureFile.length() < 1)
		return;

	wstring file = String::ToWString(textureFile);
	wstring fileName = Path::GetFileName(file);

	//   ������ �����Ѵٸ�, ��ο� �ѱ� �ȵ�
	if (Path::ExistFile(textureFile) == true)
		//   ���� ���� 
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);

	textureFile = String::ToString(fileName);
}

//   ó�� ȣ�� �ÿ��� root��尡 ��
void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	//   FbxNodeAttribute : ���� ��尡 � �����͸� ������ �ִ��� ���� ���ش�.
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	// �����Ͱ� �ִٸ�
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		//   ���� ������ �ϳ��� �����ϸ� b �� true�� �ȴ�.
		bool b = false;
		b |= (nodeType == FbxNodeAttribute::eSkeleton);
		b |= (nodeType == FbxNodeAttribute::eMesh);
		b |= (nodeType == FbxNodeAttribute::eNull);
		b |= (nodeType == FbxNodeAttribute::eMarker);

		if (b == true)
		{
			FbxBoneData * bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();
			//   Local World : �θ� ���뿡���� ��� ��ġ
			bone->LocalTransform = Utility::ToMatrix(node->EvaluateLocalTransform());
			//   World(Global) : ��ü ����
			bone->GlobalTransform = Utility::ToMatrix(node->EvaluateGlobalTransform());

			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				//   �޽��� �ﰢ�� ������ �ٲ۴�.
				converter->Triangulate
				(
					attribute,   //   ���� ����� �Ӽ�(������)
					true,      //   �ٲ㼭 ������ �־�� 
					true      //   false�� �ϸ� ���� ���� ������� �ٲ�
							  //   true�� �ϸ� ���� ���� ������� ���
				);

				//   ReadMeshData(���� ���, �޽��� �Ҽӵ� bone �ε���)
				ReadMeshData(node, index);
			}
		}//if(b)
	}

	//   �ڽ��� ���� ��ŭ ��� ȣ��
	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
}

UINT Fbx::Exporter::GetBoneIndexByName(string name)
{
	for (UINT i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}

	return 0;
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{
	//   GetMesh() : �޽��� �޾ƿ�
	//   node�� attribute�� eMesh�� �ƴϸ� NULL ��ȯ
	FbxMesh* mesh = node->GetMesh();

	vector<FbxVertex *> vertices;
	//   p : ������
	//   GetPolygonCount : ������� ����
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		//   GetPolygonSize(p) : �������� ���ؽ� ��
		int vertexInPolygon = mesh->GetPolygonSize(p);
		//   ������ 3�� �ƴϸ� assert
		assert(vertexInPolygon == 3);

		//   vertex index
		//   ������ ��ǥ��� �Ǿ��ֱ� ������ ������
		for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			//   ��Ʈ�� ����Ʈ �ε���
			//   �Ӽ��� Triangulate�Լ��� ���� �ﰢ��ȭ �ϸ� ��ȯ��
			//   Fbx�� ���� �����Ͱ� ��� �Ǿ��ִ��� �𸣱� ������
			//   Control Point�� ���� �����Ϳ� ���� �ؾ��Ѵ�.
			//   GetPolygonVertex(������ ��ȣ, ���ؽ� ��ȣ)
			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;

			D3DXVECTOR3 temp;

			//   GetControlPointAt(cpIndex)
			//   ��Ʈ�� ����Ʈ ��ȣ�� �Ѱ��ָ� �ش� ������ ��ġ�� ��ȯ�Ѵ�.
			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			//   FbxVector4 -> D3DXVECTOR3
			temp = Utility::ToVector3(position);
			//   ������ ��ǥ���� ������ �޼� ��ǥ���� �������� �ٲ�
			//   ������ ��ǥ���� ������ �״�� ����ϸ� �Ųٷ� ������ ������
			//   �ݴ��(Utility::Negative) �����´�.
			D3DXVec3TransformCoord(&vertex->Vertex.Position, &temp, &Utility::Negative());

			FbxVector4 normal;
			//   �븻 ���� �޾ƿ�
			mesh->GetPolygonVertexNormal(p, vi, normal);
			//   ������ ũ�Ⱑ �ǹ̰� ���� ������
			//   0 ~ 1���̷� ����ȭ��Ų��.
			normal.Normalize();
			temp = Utility::ToVector3(normal);
			D3DXVec3TransformCoord(&vertex->Vertex.Normal, &temp, &Utility::Negative());

			//   �ش� ��ġ�� �ִ� ������ �Ҽӵ� ���͸����� ������
			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			//   UV�� ����� ��ȣ
			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.Uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}//   for(vi)
		UINT index0 = vertices.size() - 3;
		UINT index1 = vertices.size() - 2;
		UINT index2 = vertices.size() - 1;

		D3DXVECTOR3 p0 = vertices[index0]->Vertex.Position;
		D3DXVECTOR3 p1 = vertices[index1]->Vertex.Position;
		D3DXVECTOR3 p2 = vertices[index2]->Vertex.Position;

		D3DXVECTOR2 uv0 = vertices[index0]->Vertex.Uv;
		D3DXVECTOR2 uv1 = vertices[index1]->Vertex.Uv;
		D3DXVECTOR2 uv2 = vertices[index2]->Vertex.Uv;

		D3DXVECTOR3 e0 = p1 - p0;
		D3DXVECTOR3 e1 = p2 - p0;

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;
		float r = 1.0f / (u0 * v1 - v0 * u1);

		D3DXVECTOR3 tangent;
		tangent.x = r * (v1 * e0.x - v0 * e1.x);
		tangent.y = r * (v1 * e0.y - v0 * e1.y);
		tangent.z = r * (v1 * e0.z - v0 * e1.z);

		vertices[index0]->Vertex.Tangent += tangent;
		vertices[index1]->Vertex.Tangent += tangent;
		vertices[index2]->Vertex.Tangent += tangent;

	}//   for(p)

	for (UINT i = 0; i < vertices.size(); i++)
	{
		D3DXVECTOR3 n = vertices[i]->Vertex.Normal;
		D3DXVECTOR3 t = vertices[i]->Vertex.Tangent;

		D3DXVECTOR3 temp = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&temp, &temp);

		vertices[i]->Vertex.Tangent = temp;
	}

	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone;
	meshData->Vertices = vertices;
	meshData->Mesh = mesh;
	meshDatas.push_back(meshData);
}

void Fbx::Exporter::ReadSkinData()
{
	//   ������ �޽���Ʈ ���� ����
	for (FbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;

		//   ��Ų �ִϸ��̼Ǿƴϸ� 0�� ��ȯ��
		int deformerCount = mesh->GetDeformerCount();
		//   vector.assign�� ����
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			//   TODO : cast��(����)
			FbxSkin* skin = reinterpret_cast<FbxSkin *>(deformer);
			if (skin == NULL) continue;

			//   Cluster : �������
			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

				//   cluster�� ������ ���� ���� �̸�
				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);

				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);
				
				//   cluster�� ������ �޴� ���� ����
				boneDatas[boneIndex]->LocalTransform = Utility::ToMatrix(transform);
				boneDatas[boneIndex]->GlobalTransform = Utility::ToMatrix(linkTransform);

				//   cluster�� ������ �޴� �ε��� ��ȣ
				//   �������� �ݺ������� ���ü��� �ִ�.
				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddWeights(boneIndex, (float)weights[indexCount]);
				}
			}// for(cluster index)
		}//   for(deformer)

		for (UINT i = 0; i < boneWeights.size(); i++)
			boneWeights[i].Normalize();

		for (FbxVertex* vertex : meshData->Vertices)
		{
			int cpIndex = vertex->ControlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeights(weights);
			vertex->Vertex.BlendIndices = weights.Indices;
			vertex->Vertex.BlendWeights = weights.Weights;
		}

		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial * material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex *> gather;
			for (FbxVertex* temp : meshData->Vertices)
			{
				//   �ش� ���͸��� �ִ� ���ؽ� ����
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;

			//   ���� �׷��� ����
			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size());
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}//   for(MeshData)
}

void Fbx::Exporter::WriteMesh(wstring saveFolder, wstring fileName)
{
	//   ���� ����
	Path::CreateFolder(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	//   ������ ���� ����
	w->Open(saveFolder + fileName);

	//   �������Ͱ� � ����
	w->UInt(boneDatas.size());
	for (FbxBoneData * bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);

		w->Matrix(bone->LocalTransform);
		w->Matrix(bone->GlobalTransform);

		SAFE_DELETE(bone);
	}

	w->UInt(meshDatas.size());
	for (FbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (FbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			//   ������ ��ü�� �ּ��� ����
			//   ������ 0���� �ּҸ� ��������
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close();
	SAFE_DELETE(w);
}

Fbx::FbxClip* Fbx::Exporter::ReadAnimationData(wstring name)
{
	UINT number = -1;
	//   GetAnimStackCount : �ִϸ��̼� ����
	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		//   GetTakeInfo : ����
		// �ش� ������ �̸��� �޾Ƽ� �޾ƿ� �̸��� ���� 
		// ������ number�� �ε�������
		FbxTakeInfo *takeInfo = importer->GetTakeInfo(i);
		wstring compare = String::ToWString(takeInfo->mName.Buffer());

		if (name == compare)
		{
			number = i;

			break;
		}
	}

	if (number != -1)
		return ReadAnimationData(number);

	return NULL;
}

Fbx::FbxClip* Fbx::Exporter::ReadAnimationData(UINT index)
{
	UINT stackCount = (UINT)importer->GetAnimStackCount();
	assert(index < stackCount);

	//   GetGlobalSettings : ���� ���� �Ǿ��ִ� ��� ������ ������
	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
	// �ӵ��� �޾ƿ�
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	//���ϸ��̼� Ŭ�������� ����
	FbxClip *clip = new FbxClip();
	clip->FrameRate = frameRate;

	//   GetTakeInfo : ����
	// ��ȣ�� �ش��ϴ� ����
	FbxTakeInfo *takeInfo = importer->GetTakeInfo(index);
	clip->Name = takeInfo->mName.Buffer();

	//   ���� �ִϸ��̼��� �ð� ����
	FbxTimeSpan span = takeInfo->mLocalTimeSpan;
	FbxTime tempDuration = span.GetDuration();

	int start = (int)span.GetStart().GetFrameCount();		//���ϸ��̼� ���۽ð�
	int stop = (int)span.GetStop().GetFrameCount();			//���ϸ��̼� ����ð�
	float duration = (float)tempDuration.GetMilliSeconds();	//���ϸ��̼� ���ӽð�

	if (start < stop)
		ReadKeyframeData(clip, scene->GetRootNode(), start, stop,-1,-1);

	clip->Duration = duration;
	clip->FrameCount = (stop - start) + 1;	//������ ���ۻ����� 1�����ִ°�

	return clip;
}

//��� ���� Ű������ �����͸� �д´�
void Fbx::Exporter::ReadKeyframeData(struct FbxClip* clip, FbxNode* node, int start, int stop, int index, int parent)
{
	//����� ������ �����´�
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		//���Ÿ���� ���̷����϶�
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			//Ű������ ����
			FbxKeyFrame* keyframe = new FbxKeyFrame();
			keyframe->BoneName = node->GetName();
			keyframe->index = index;
			keyframe->parent = parent;

			//�ð��� �������Ӿ� Ű������ �����͸� �����Ѵ�
			for (int i = start; i <= stop; i++)
			{
				//   SetFrame : 0~frame�� �ð� ���
				FbxTime animationTime;
				animationTime.SetFrame(i);
				//   TODO : affine ��ȯ
				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				D3DXMATRIX transform = Utility::ToMatrix(matrix);

				FbxKeyframeData data;
				data.Time = (float)animationTime.GetMilliSeconds();
				D3DXMatrixDecompose(&data.Scale, &data.Rotation, &data.Translation, &transform);

				keyframe->Transforms.push_back(data);
			}
			//Ŭ���� Ű������ ����
			clip->KeyFrames.push_back(keyframe);
		}//if(nodeType)
	}
	//�ڽ��� ������ŭ �ݺ� => �㸮->�ٸ�->�� �̷�������
	for (int i = 0; i < node->GetChildCount(); i++)
		ReadKeyframeData(clip, node->GetChild(i), start, stop,clip->KeyFrames.size(),index);
}

void Fbx::Exporter::WriteClipData(FbxClip * clip, wstring saveFolder, wstring fileName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + fileName);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->KeyFrames.size());
	for (FbxKeyFrame* keyframe : clip->KeyFrames)
	{
		w->String(keyframe->BoneName);
		w->UInt(keyframe->index);
		w->UInt(keyframe->parent);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(FbxKeyframeData) * keyframe->Transforms.size());

		SAFE_DELETE(keyframe);
	}

	w->Close();
	SAFE_DELETE(w);
}