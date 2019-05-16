#include "Framework.h"
#include "Model.h"

void Model::ReadMaterial(wstring file)
{
	Models::LoadMaterial(file, &materials);
}

void Model::ReadMesh(wstring file)
{
	isFirstName=Models::LoadMesh(file, &bones, &meshes);
	modelName = Path::GetFileName(file);

	BindingBone();
	BindingMesh();
}

void Model::BindingBone()
{
	this->root = bones[0];
	for (ModelBone * bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
		{
			bone->parent = NULL;
		}
	}
}

void Model::BindingMesh()
{
	D3DXVECTOR3 minPoint = { 0,0,0 };
	D3DXVECTOR3 maxPoint = { 0,0,0 };

	for (ModelMesh * mesh : meshes)
	{
		for (ModelBone * bone : bones)
		{
			if (mesh->parentBoneIndex == bone->index)
			{
				mesh->parentBone = bone;

				break;
			}
		}

		for (UINT i = 0; i < mesh->VertexCount(); i++)
		{

			D3DXVECTOR3 ver = mesh->Vertices()[i].Position;
			D3DXVec3TransformCoord(&ver, &mesh->Vertices()[i].Position, &mesh->ParentBone()->global);

			if (minPoint.x > ver.x)minPoint.x = ver.x;
			if (minPoint.y > ver.y)minPoint.y = ver.y;
			if (minPoint.z > ver.z)minPoint.z = ver.z;

			if (maxPoint.x < ver.x)maxPoint.x = ver.x;
			if (maxPoint.y < ver.y)maxPoint.y = ver.y;
			if (maxPoint.z < ver.z)maxPoint.z = ver.z;
		}


		for (ModelMeshPart * part : mesh->meshParts)
		{
			for (Material * material : materials)
			{
				if (material->Name() == part->materialName)
				{
					part->material = material;

					break;
				}
			}
		}
		//   메쉬에 속한 모델 파트의 정점과 인덱스를
		//   버퍼에 담는다.
		mesh->Binding();
	}

	min = minPoint;
	max = maxPoint;

	float length = fabs(max.x - min.x);

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z)); // 왼쪽하단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z)); // 왼쪽상단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z)); // 오른쪽하단앞쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z)); // 오른쪽상단앞쪽

	pickCube.push_back(D3DXVECTOR3(min.x, min.y, min.z + length)); // 왼쪽하단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x, min.y + length, min.z + length)); // 왼쪽상단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y, min.z + length)); // 오른쪽하단뒷쪽
	pickCube.push_back(D3DXVECTOR3(min.x + length, min.y + length, min.z + length)); // 오른쪽상단뒷쪽
}


// --------------------------------------------------------------------- //
//   Model Manager
// --------------------------------------------------------------------- //
map<wstring, vector<Material*>>Models::materialMap;
void Models::LoadMaterial(wstring file, vector<Material*>* materials)
{
	if (materialMap.count(file) < 1)
		ReadMaterialData(file);

	for (Material * material : materialMap[file])
	{
		Material * temp = NULL;
		material->Clone((void**)&temp);

		materials->push_back(temp);
	}
}

void Models::ReadMaterialData(wstring file)
{
	vector<Material*> materials;

	Xml::XMLDocument * document = new Xml::XMLDocument();

	wstring tempFile = file;
	//   파일을 연다.
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	//   SML_SUCCESS가 반환되면 파일 열기 성공
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement * root = document->FirstChildElement();
	Xml::XMLElement * matNode = root->FirstChildElement();

	do
	{
		Xml::XMLElement* node = NULL;

		Material* material = new Material();

		node = matNode->FirstChildElement();
		material->Name(String::ToWString(node->GetText()));


		wstring directory = Path::GetDirectoryName(tempFile);

		node = node->NextSiblingElement();
		wstring diffuseTexture = String::ToWString(node->GetText());
		if (diffuseTexture.length() > 0)
			material->SetDiffuseMap(directory + diffuseTexture);

		node = node->NextSiblingElement();
		wstring specularTexture = String::ToWString(node->GetText());
		if (specularTexture.length() > 0)
			material->SetSpecularMap(directory + specularTexture);

		node = node->NextSiblingElement();
		wstring normalTexture = String::ToWString(node->GetText());
		if (normalTexture.length() > 0)
			material->SetNormalMap(directory + normalTexture);


		D3DXCOLOR dxColor;
		Xml::XMLElement* color;

		//DiffuseColor
		node = node->NextSiblingElement();
		color = node->FirstChildElement();
		dxColor.r = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.g = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.b = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.a = color->FloatText();
		material->SetDiffuse(dxColor);


		//SpecularColor
		node = node->NextSiblingElement();
		color = node->FirstChildElement();
		dxColor.r = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.g = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.b = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.a = color->FloatText();
		material->SetSpecular(dxColor);


		node = node->NextSiblingElement();

		D3DXCOLOR specular;
		specular=material->GetSpecular();
		specular.a = node->FloatText();
		material->SetSpecular(specular);

		materials.push_back(material);

		matNode = matNode->NextSiblingElement();
	} while (matNode != NULL);

	materialMap[file] = materials;

	SAFE_DELETE(document);
}

map<wstring, Models::MeshData> Models::meshDataMap;
map<wstring, bool> Models::firstNameMap;

bool Models::LoadMesh(wstring file, vector<class ModelBone*>* bones, vector<class ModelMesh*>* meshes)
{
	bool isFirstName = false;
	if (meshDataMap.count(file) < 1)
		ReadMeshData(file);

	isFirstName = firstNameMap[file];
	MeshData data = meshDataMap[file];
	for (size_t i = 0; i < data.Bones.size(); i++)
	{
		ModelBone * bone = NULL;
		data.Bones[i]->Clone((void**)&bone);

		bones->push_back(bone);
	}

	for (size_t i = 0; i < data.Meshes.size(); i++)
	{
		ModelMesh * mesh = NULL;
		data.Meshes[i]->Clone((void**)&mesh);

		meshes->push_back(mesh);
	}

	return isFirstName;
}

bool Models::ReadMeshData(wstring file)
{
	firstNameMap[file] = false;

	//   저장한 순서 그대로 읽어야 한다.
	BinaryReader *r = new BinaryReader();
	r->Open(file);

	vector<ModelBone *>bones;
	vector<ModelMesh *>meshes;

	UINT count = 0;
	count = r->UInt();

	bool isCheckBone = false;
	vector<wstring> boneNames;

	for (UINT i = 0; i < count; i++)
	{
		ModelBone *bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());

		boneNames.push_back(bone->name);


		UINT checkBoneIdx = 0;
		bool isCheckFirstBone = false;
		//포문을 돌아야한다 근데 세미콜론만나면 탈출
		for (UINT i = 0; i < boneNames[boneNames.size()-1].size(); i++)
		{
			if (boneNames[boneNames.size() - 1][i] == L':')
			{
				isCheckFirstBone = true;
				break;
			}

			checkBoneIdx++;
		}


		if(isCheckFirstBone)
			boneNames[boneNames.size() - 1].erase(boneNames[boneNames.size() - 1].begin(), boneNames[boneNames.size() - 1].end() - (boneNames[boneNames.size() - 1].size()-checkBoneIdx-1));
		
		bone->parentIndex = r->Int();

		bone->local = r->Matrix();
		bone->global = r->Matrix();

		bones.push_back(bone);
	}

	if (boneNames[0]!= L"Hips")
	{
		firstNameMap[file] = true;
	}
	
	boneNames[0];

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh *mesh = new ModelMesh();
		mesh->name = String::ToWString(r->String());


		mesh->parentBoneIndex = r->Int();

		//	VertexData
		{
			UINT count = r->UInt();
			//	vector.assign(n, x) : vector에 x의 값으로 n개의 원소를 할당
			//	vector.reserve(n) : n개의 원소를 저장할 공간을 예약
			vector<VertexTextureNormalTangentBlend> vertices;
			vertices.assign(count, VertexTextureNormalTangentBlend());

			void* ptr = (void *)&(vertices[0]);
			r->Byte(&ptr, sizeof(VertexTextureNormalTangentBlend) * count);

			mesh->vertices = new VertexTextureNormalTangentBlend[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<VertexTextureNormalTangentBlend *>(mesh->vertices, count)
			);
		}

		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart *meshPart = new ModelMeshPart();
			meshPart->parent = mesh;
			meshPart->materialName = String::ToWString(r->String());

			meshPart->startVertex = r->UInt();
			meshPart->vertexCount = r->UInt();

			mesh->meshParts.push_back(meshPart);
		}//	for(k)

		meshes.push_back(mesh);
	}//	for(i)


	r->Close();
	SAFE_DELETE(r);

	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());

	meshDataMap[file] = data;

	return firstNameMap[file];
}