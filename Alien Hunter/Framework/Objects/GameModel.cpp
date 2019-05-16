#include "Framework.h"
#include "GameModel.h"

#include "Model/Model.h"
#include "Model/ModelMesh.h"
#include "Model/ModelMeshPart.h"

#include "Collider\ColliderBox.h"
#include "Collider\ColliderRay.h"
#include "Collider\ColliderSphere.h"

GameModel::GameModel(Effect*lineEffect)
	:lineEffect(lineEffect),pass(0),trailEffect(NULL)
{
	typeName = L"Model";
	Name()= L"Model";
	isLoad = false;
}

GameModel::GameModel(UINT instanceId, wstring matFile, wstring meshFile, Effect*lineEffect)
	:lineEffect(lineEffect),matFile(matFile), meshFile(meshFile),
	pass(0), instanceID(instanceId), trailEffect(NULL)
{
	typeName = L"Model";
	Name() = L"Model";
	isLoad = false;
}

GameModel::GameModel(GameModel & copy)
{
	Name() = L"Model";
	lineEffect = copy.lineEffect;
	instanceID = copy.instanceID;
	matFile = copy.matFile;
	meshFile = copy.meshFile;
	loadMaterial.assign(copy.loadMaterial.begin(), copy.loadMaterial.end());
	isLoad = false;
	pass = 0;
	trailEffect = NULL;

	colliders.assign(copy.colliders.begin(), copy.colliders.end());

	LoadPosition() = copy.LoadPosition();
	LoadRotate() = copy.LoadRotate();
	LoadScale() = copy.LoadScale();

	/*colliders.resize(copy.colliders.size());

	for (size_t i = 0; i < copy.colliders.size(); i++)
	{
		if (copy.colliders[i]->Type() == (UINT)ColliderElement::EType::EBox)
		{
			ColliderBox*copyCollider = dynamic_cast<ColliderBox*>(copy.colliders[i]);
			ColliderBox*box = new ColliderBox(*copyCollider);

			colliders[i] = box;
		}
		else if (copy.colliders[i]->Type() == (UINT)ColliderElement::EType::ESphere)
		{
			ColliderSphere*copyCollider = dynamic_cast<ColliderSphere*>(copy.colliders[i]);
			ColliderSphere*box = new ColliderSphere(*copyCollider);

			colliders[i] = box;
		}
		else if (copy.colliders[i]->Type() == (UINT)ColliderElement::EType::ERay)
		{
			ColliderRay*copyCollider = dynamic_cast<ColliderRay*>(copy.colliders[i]);
			ColliderRay*box = new ColliderRay(*copyCollider);

			colliders[i] = box;
		}
	}*/
	typeName = L"Model";
	LoadDataFile() = copy.LoadDataFile();
	Name() = copy.Name();

}

void GameModel::DeleteModel()
{
	SAFE_DELETE(model);
}

GameModel::~GameModel()
{
	//모델삭제는 인스턴스에서하기

	for (size_t i = 0; i < colliders.size(); i++)
	{
		SAFE_DELETE(colliders[i]);
	}

	for (size_t i = 0; i < loadMaterial.size(); i++)
	{
		SAFE_DELETE(loadMaterial[i]);
	}

	SAFE_DELETE(trailEffect);
}

void GameModel::ResetModelData(UINT instID, wstring matFile, wstring meshFile)
{
	instanceID = instID;
	this->matFile = matFile;
	this->meshFile = meshFile;
}

void GameModel::Ready()
{
	//모델만드는것도 인스턴스에이미 모델이있으면 만들지말고 인스턴스
	model = new Model();
	model->ReadMaterial(this->matFile);
	model->ReadMesh(this->meshFile);


	for (size_t i = 0; i < loadMaterial.size(); i++)
	{
		if(loadMaterial[i]->GetDiffuseMap()!=NULL)
			model->Materials()[i]->SetDiffuseMap(loadMaterial[i]->GetDiffuseMap()->GetFile());

		if (loadMaterial[i]->GetSpecularMap() != NULL)
			model->Materials()[i]->SetSpecularMap(loadMaterial[i]->GetSpecularMap()->GetFile());

		if (loadMaterial[i]->GetNormalMap() != NULL)
			model->Materials()[i]->SetNormalMap(loadMaterial[i]->GetNormalMap()->GetFile());

		model->Materials()[i]->SetDiffuse(loadMaterial[i]->GetDiffuse());
		model->Materials()[i]->SetSpecular(loadMaterial[i]->GetSpecular());
	}
	

	if (InstanceManager::Get()->FindModel(model->GetModelName())!=NULL)
	{
		wstring modelName = model->GetModelName();
		SAFE_DELETE(model);
		model = InstanceManager::Get()->FindModel(modelName)->GetModel();
	}

	if(!isLoad)
		InstanceManager::Get()->AddModel(this, Effects + L"019_ModelInstance.hlsl",InstanceManager::InstanceType::MODEL);
	
	UpdateWorld();

	Position(LoadPosition());
	Rotation(LoadRotate());
	Scale(LoadScale());

	if (trailEffect == NULL)
		trailEffect = new Effect(Effects + L"021_TrailRender.hlsl");

	for (size_t i = 0; i < trailDesc.size(); i++)
	{
		TrailRenderer*trail = new TrailRenderer(this, trailEffect);
		SAFE_DELETE(trail->GetDiffuseMap());
		trail->GetDiffuseMap() = new Texture(trailDesc[i].diffuseMap);

		SAFE_DELETE(trail->GetAlphaMap());
		trail->GetAlphaMap() = new Texture(trailDesc[i].alphaMap);

		trail->SetBone(trailDesc[i].boneIdx);
		trail->StartPos()._41 = trailDesc[i].startPos.x;
		trail->StartPos()._42 = trailDesc[i].startPos.y;
		trail->StartPos()._43 = trailDesc[i].startPos.z;

		trail->EndPos()._41 = trailDesc[i].endPos.x;
		trail->EndPos()._42 = trailDesc[i].endPos.y;
		trail->EndPos()._43 = trailDesc[i].endPos.z;

		trail->SetLifeTime(trailDesc[i].lifeTime);
		trail->SetRun(true);
		trails.push_back(trail);
	}


}

void GameModel::Update()
{
	for (size_t i = 0; i < trails.size(); i++)
	{
		trails[i]->Update();
	}
}

void GameModel::Render()
{
	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->DrawCollider();
	}

	for (size_t i = 0; i < trails.size(); i++)
	{
		trails[i]->Render();
	}
}

GData * GameModel::Save()
{
	ModelData*data=new ModelData();

	data->modelName = model->GetModelName();
	data->type = typeName;
	data->meshFile = meshFile;
	data->materialFile = matFile;
	data->materials.assign(model->Materials().begin(), model->Materials().end());
	data->min = model->Min();
	data->max = model->Max();
	data->colliders.assign(colliders.begin(), colliders.end());
	data->loadFileData = LoadDataFile();

	data->trails.assign(trails.begin(), trails.end());

	Position(&data->loadPosition);
	Scale(&data->loadScale);
	Rotation(&data->loadRotate);

	return (GData*)data;
}

void GameModel::Load(wstring fileName)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	LoadDataFile() = fileName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement();
	node = node->NextSiblingElement(); 
	meshFile = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); 
	matFile = String::ToWString(node->GetText());

	node = node->NextSiblingElement();
	wstring name = String::ToWString(node->GetText())+to_wstring(instanceID); // Name

	D3DXVECTOR3 Min, Max;
	node = node->NextSiblingElement(); // MinX
	Min.x = node->FloatText();

	node = node->NextSiblingElement(); // MinY
	Min.y = node->FloatText();

	node = node->NextSiblingElement(); // MinZ
	Min.z = node->FloatText();

	node = node->NextSiblingElement(); // MaxX
	Max.x = node->FloatText();

	node = node->NextSiblingElement(); // MaxY
	Max.y = node->FloatText();

	node = node->NextSiblingElement(); // MaxZ
	Max.z = node->FloatText();

	node = node->NextSiblingElement(); // MaterialSize
	UINT materialSize = node->IntText();

	node = node->NextSiblingElement(); // material

	for (UINT i = 0; i < materialSize; i++)
	{
		Xml::XMLElement*materialNode = node->FirstChildElement();
		Material*material = new Material;
		
		wstring diffuseFile = String::ToWString(materialNode->GetText());
	/*	if (diffuseFile != L"")
			material->SetDiffuseMap(diffuseFile);*/

		materialNode = materialNode->NextSiblingElement(); // SpecularFile
		wstring specularFile = String::ToWString(materialNode->GetText());
		//if(specularFile!=L"")
		//	material->SetSpecularMap(specularFile);

		materialNode = materialNode->NextSiblingElement(); // NormalFile
		wstring normalFile = String::ToWString(materialNode->GetText());
		//if (normalFile != L"")
		//	material->SetNormalMap(normalFile);

		materialNode = materialNode->NextSiblingElement(); // Diffuse
		{
			D3DXCOLOR diffuse;
			Xml::XMLElement*diffuseNode = materialNode->FirstChildElement(); // DiffuseR
			diffuse.r = diffuseNode->FloatText();

			diffuseNode->NextSiblingElement(); // DiffuseG
			diffuse.g = diffuseNode->FloatText();

			diffuseNode->NextSiblingElement(); // DiffuseB
			diffuse.b = diffuseNode->FloatText();

			diffuseNode->NextSiblingElement(); // DiffuseA
			diffuse.a = diffuseNode->FloatText();
			material->SetDiffuse(diffuse);
		}

		materialNode = materialNode->NextSiblingElement(); // Specular
		{
			D3DXCOLOR specular;
			Xml::XMLElement*specularNode = materialNode->FirstChildElement(); // SpecularR
			specular.r = specularNode->FloatText();

			specularNode->NextSiblingElement(); // SpecularG
			specular.g = specularNode->FloatText();

			specularNode->NextSiblingElement(); // SpecularB
			specular.b = specularNode->FloatText();

			specularNode->NextSiblingElement(); // SpecularA
			specular.a = specularNode->FloatText();
			material->SetSpecular(specular);
		}
		loadMaterial.push_back(material);

		node = node->NextSiblingElement();
	}

	UINT colliderSize = node->IntText();// ColliderSize

	node = node->NextSiblingElement(); // Collider

	for (UINT i = 0; i < colliderSize; i++)
	{
		Xml::XMLElement*colliderNode = node->FirstChildElement();
		wstring typeName = String::ToWString(colliderNode->GetText());
		
		if (typeName == L"Box")
		{
			ColliderBox*boxCollider = new ColliderBox(Name() + L"Box" + to_wstring(colliders.size())+to_wstring(instanceID), Min, Max, lineEffect);
			boxCollider->Ready();

			colliders.push_back(boxCollider);
		}
		else if (typeName == L"Sphere")
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(Min, Max);

			float radius = (Min.x - Max.x) / 2.0f;

			ColliderSphere*sphereCollider = new ColliderSphere(Name() + L"Sphere" + to_wstring(colliders.size()) + to_wstring(instanceID), centerPos, radius, lineEffect);
			sphereCollider->Ready();

			colliders.push_back(sphereCollider);
		}
		else if (typeName == L"Ray")
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(Min, Max);

			ColliderRay*rayCollider = new ColliderRay(Name() + L"Ray" + to_wstring(colliders.size()) + to_wstring(instanceID), centerPos, Direction(), lineEffect);
			rayCollider->Ready();

			colliders.push_back(rayCollider);
		}

		D3DXVECTOR3 position, scale, rotate;
		float distance,radius;
		D3DXVECTOR3 dir;

		colliderNode = colliderNode->NextSiblingElement(); // ColliderPosX
		position.x = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderPosY
		position.y = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderPosZ
		position.z = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderScaleX
		scale.x = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderScaleY
		scale.y = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderScaleZ
		scale.z = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderRotateX
		rotate.x = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderRotateY
		rotate.y = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement(); // ColliderRotateZ
		rotate.z = colliderNode->FloatText();

		colliders[i]->Scale(scale);
		colliders[i]->Rotation(rotate);
		colliders[i]->Position(position);

		if (typeName == L"Sphere")
		{
			ColliderSphere*ray = dynamic_cast<ColliderSphere*>(colliders[i]);
			colliderNode = colliderNode->NextSiblingElement(); // Distance
			radius = colliderNode->FloatText();

			ray->Radius() = radius;
		}
		else if (typeName == L"Ray")
		{
			ColliderRay*ray = dynamic_cast<ColliderRay*>(colliders[i]);

			colliderNode = colliderNode->NextSiblingElement(); // Distance
			distance = colliderNode->FloatText();

			colliderNode = colliderNode->NextSiblingElement(); // DirectionX
			dir.x = colliderNode->FloatText();

			colliderNode = colliderNode->NextSiblingElement(); // DirectionY
			dir.y = colliderNode->FloatText();

			colliderNode = colliderNode->NextSiblingElement(); // DirectionZ
			dir.z = colliderNode->FloatText();

			ray->GetRay()->Distance = distance;
			ray->GetRay()->Direction = dir;
		}

		node = node->NextSiblingElement();
	}

	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(World());
	}

	UINT trailSize = node->IntText();// TrailSize

	node = node->NextSiblingElement(); // Trail

	for (UINT i = 0; i < trailSize; i++)
	{
		Xml::XMLElement*colliderNode = node->FirstChildElement();
		float lifeTime = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float startX = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float startY = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float startZ = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float endX = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float endY = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		float endZ = colliderNode->FloatText();

		colliderNode = colliderNode->NextSiblingElement();
		wstring diffuseMap = String::ToWString(colliderNode->GetText());

		colliderNode = colliderNode->NextSiblingElement();
		wstring alphaMap = String::ToWString(colliderNode->GetText());

		colliderNode = colliderNode->NextSiblingElement();
		int boneIdx = colliderNode->IntText();

		TrailDesc tdesc;
		tdesc.lifeTime = lifeTime;
		tdesc.boneIdx = boneIdx;
		tdesc.startPos = { startX,startY,startZ };
		tdesc.endPos = { endX,endY,endZ };
		tdesc.diffuseMap = diffuseMap;
		tdesc.alphaMap = alphaMap;
		
		trailDesc.push_back(tdesc);

		node = node->NextSiblingElement();
	}

	SAFE_DELETE(document);
}

#pragma region Material
void GameModel::SetShader(wstring shaderFiler)
{
	for (Material* material : model->Materials())
		material->SetEffect(shaderFiler);
}

void GameModel::SetShader(Effect * effect)
{
	for (Material* material : model->Materials())
		material->SetEffect(effect);
}

void GameModel::SetDiffuse(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetDiffuse(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(color);
}

void GameModel::SetDiffuseMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDiffuseMap(file);
}

void GameModel::SetSpecular(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetSpecular(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetSpecular(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetSpecular(color);
}

void GameModel::SetSpecularMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetSpecularMap(file);
}

void GameModel::SetNormalMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetNormalMap(file);
}

void GameModel::SetShader(D3DXMATRIX v, D3DXMATRIX p)
{
	for (Material * material :GetModel()->Materials())
	{
		material->GetEffect()->AsMatrix("LightView")->SetMatrix(v);
		material->GetEffect()->AsMatrix("LightProjection")->SetMatrix(p);
	}
}

void GameModel::SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv)
{
	for (Material * material : GetModel()->Materials())
	{
		material->GetEffect()->AsMatrix("ShadowTransform")->SetMatrix(shadowTransform);

		if (srv != NULL)
			material->GetEffect()->AsSRV("ShadowMap")->SetResource(srv);
	}
}

#pragma endregion

bool GameModel::IsPicked()
{
	return isPicked;
}
void GameModel::SetPickState(bool val)
{
	isPicked = val;
}

vector<ColliderElement*>& GameModel::GetColliders()
{
	return colliders;
}

void GameModel::CreateCollider(int type)
{
	switch (type)
	{
		case 0:
		{
			ColliderBox*boxCollider = new ColliderBox(Name() + L"Box" + to_wstring(colliders.size()), model->Min(), model->Max(), lineEffect);
			boxCollider->Ready();

			colliders.push_back(boxCollider);
		}
		break;
		case 1:
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(model->Min(), model->Max());

			ColliderRay*rayCollider = new ColliderRay(Name() + L"Ray" + to_wstring(colliders.size()), centerPos, Direction(), lineEffect);
			rayCollider->Ready();

			colliders.push_back(rayCollider);
		}
		break;
		case 2:
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(model->Min(), model->Max());


			float radius = (model->Max().x - model->Min().x) / 2.0f;

			ColliderSphere*sphereCollider = new ColliderSphere(Name() + L"Sphere" + to_wstring(colliders.size()), centerPos, radius, lineEffect);
			sphereCollider->Ready();

			colliders.push_back(sphereCollider);
		}
		break;
	}
	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(World());
	}
}

void GameModel::ColliderUpdate()
{

}

void GameModel::PickUpdate()
{
	if (Mouse::Get()->Down(2))
	{
		D3DXVECTOR3 start;
		Context::Get()->GetMainCamera()->Position(&start);

		D3DXVECTOR3 direction = Context::Get()->GetMainCamera()->Direction
		(Context::Get()->GetViewport(), Context::Get()->GetPerspective());

		D3DXMATRIX inverse;
		D3DXMATRIX mWorld;
		D3DXMatrixIdentity(&mWorld);
		mWorld._41 = World()._41;
		mWorld._42 = World()._42;
		mWorld._43 = World()._43;

		D3DXMatrixInverse(&inverse, NULL, &mWorld);

		D3DXVec3TransformNormal(&direction, &direction, &inverse);

		//내가구한 최대최소점의 면과 선이 충돌했을때 트루반환

		D3DXVECTOR3 minpos, maxpos;
		float length = fabs(model->Max().x - model->Min().x);

		minpos = { 0,0,0 };
		maxpos = { 0,0,0 };

		vector<D3DXVECTOR3> checkCube;
		checkCube.clear();


		checkCube.resize(8);
		for (int i = 0; i < 8; i++)
			D3DXVec3TransformCoord(&checkCube[i], &model->PickCube()[i], &World());


		vector<UINT> indices;
		indices.resize(36);
		indices = {
			0,1,2,2,1,3,
			2,3,6,6,3,7,
			6,7,4,4,7,5,
			0,1,4,4,1,5,
			1,5,3,3,5,7,
			0,4,2,2,4,6
		};

		for (int i = 0; i < 6; i++)
		{
			D3DXVECTOR3 p[6];
			for (int j = 0; j < 6; j++)
				p[j] = checkCube[indices[6 * i + j]];

			float u, v, distance;

			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				if (!isPicked)
					isPicked = true;
				else
					isPicked = false;
				break;

			}
			else if (D3DXIntersectTri(&p[3], &p[4], &p[5], &start, &direction, &u, &v, &distance))
			{
				if (!isPicked)
					isPicked = true;
				else
					isPicked = false;
				break;
			}
		}
	}
}

D3DXMATRIX GameModel::GetPlayerBoneWorld(int idx)
{
	return World();
}

void GameModel::UpdateWorld()
{
	__super::UpdateWorld();

	InstanceManager::Get()->UpdateWorld(this, instanceID, World(), InstanceManager::InstanceType::MODEL);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(World());
	}
}


D3DXMATRIX GameModel::GetBoneMatrix(int idx)
{
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 0.01f, 0.01f, 0.01f);
	D3DXMATRIX matrix;// = renderTransforms[idx] * S;

	return matrix;
}
