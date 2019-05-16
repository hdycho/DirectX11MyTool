#include "Framework.h"
#include "FileManager.h"
#include "Datas\GameData.h"
#include "Collider\ColliderRay.h"
#include "Collider\ColliderSphere.h"
#include "Model\ModelClip.h"

#include "Environment\Sky.h"
#include "Environment\Terrain.h"
#include "Environment\Billboard.h"
#include "Environment\Rain.h"
#include "Environment\Shadow.h"

FileManager*FileManager::instance = NULL;

void FileManager::Create()
{
	if (instance == NULL)
		instance = new FileManager();
}

void FileManager::Delete()
{
	if (instance != NULL)
	{
		DataIter diter = instance->dataMap.begin();
		for (; diter != instance->dataMap.end(); diter++)
		{
			diter->second.clear();
		}
	}
	instance->dataMap.clear();
	SAFE_DELETE(instance);
}

void FileManager::DataPush(FileType type, IData * data)
{
	dataMap[type].push_back(data);
}

void FileManager::Erase(IData * obj)
{
	DataIter diter = dataMap.begin();

	for (; diter != dataMap.end(); diter++)
	{
		vector<class IData*>::iterator viter = diter->second.begin();

		for (; viter != diter->second.end();)
		{
			if (*viter == obj)
			{
				diter->second.erase(viter);
			}
			else
				viter++;
		}
		if (diter->second.size() < 1)
		{
			dataMap.erase(diter);
			break;
		}
	}
}

void FileManager::Save(wstring fileName)
{
	//xml열어서 세이브하는식으로 하면될듯
	DataIter diter = dataMap.begin();

	for (; diter != dataMap.end(); diter++)
	{
		switch (diter->first)
		{
			case FileType::Sky:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					SkyData*data = (SkyData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("SkyDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Name");
					element->SetText("Sky");
					node->LinkEndChild(element);

					element = document->NewElement("StarMap");
					element->SetText(String::ToString(data->starMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MoonMap");
					element->SetText(String::ToString(data->MoonMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("GlowMap");
					element->SetText(String::ToString(data->glowMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("CloudMap");
					element->SetText(String::ToString(data->cloudMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Leans");
					element->SetText(data->leans);
					node->LinkEndChild(element);

					element = document->NewElement("TimeFactor");
					element->SetText(data->timeFactor);
					node->LinkEndChild(element);

					element = document->NewElement("SampleCount");
					element->SetText(data->sampleCount);
					node->LinkEndChild(element);

					element = document->NewElement("CloudScale");
					element->SetText(data->cloudScale);
					node->LinkEndChild(element);

					element = document->NewElement("CloudSpeed");
					element->SetText(data->cloudSpeed);
					node->LinkEndChild(element);

					element = document->NewElement("WaveLength");
					node->LinkEndChild(element);

					Xml::XMLElement * wave = NULL;
					wave = document->NewElement("WaveLengthX");
					wave->SetText(data->waveLength.x);
					element->LinkEndChild(wave);

					wave = document->NewElement("WaveLengthY");
					wave->SetText(data->waveLength.y);
					element->LinkEndChild(wave);

					wave = document->NewElement("WaveLengthZ");
					wave->SetText(data->waveLength.z);
					element->LinkEndChild(wave);

					node->LinkEndChild(element);
					element = document->NewElement("StartTheta");
					element->SetText(data->startTheta);
					node->LinkEndChild(element);

					string file = String::ToString(Datas + fileName + L"/Sky0.sky");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Terrain:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					TerrainData*data = (TerrainData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("TerrainDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Name");
					element->SetText("Terrain");
					node->LinkEndChild(element);

					element = document->NewElement("HeightMap");
					element->SetText(String::ToString(data->heightMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("HeightScale");
					element->SetText(data->heightScale);
					node->LinkEndChild(element);

					element = document->NewElement("HeightMapWidth");
					element->SetText(data->heightMapWidth);
					node->LinkEndChild(element);

					element = document->NewElement("HeightMapHeight");
					element->SetText(data->heightMapHeight);
					node->LinkEndChild(element);

					element = document->NewElement("LayerMaps");
					node->LinkEndChild(element);

					Xml::XMLElement * layerMap = NULL;
					layerMap = document->NewElement("layer1");
					layerMap->SetText(String::ToString(data->layerMap[0]).c_str());
					element->LinkEndChild(layerMap);

					layerMap = document->NewElement("layer2");
					layerMap->SetText(String::ToString(data->layerMap[1]).c_str());
					element->LinkEndChild(layerMap);

					layerMap = document->NewElement("layer3");
					layerMap->SetText(String::ToString(data->layerMap[2]).c_str());
					element->LinkEndChild(layerMap);

					element = document->NewElement("BlendHeight");
					node->LinkEndChild(element);

					Xml::XMLElement * blendHeight = NULL;
					blendHeight = document->NewElement("blendHeight1");
					blendHeight->SetText(data->blendHeight.x);
					element->LinkEndChild(blendHeight);

					blendHeight = document->NewElement("blendHeight2");
					blendHeight->SetText(data->blendHeight.y);
					element->LinkEndChild(blendHeight);

					blendHeight = document->NewElement("blendHeight3");
					blendHeight->SetText(data->blendHeight.z);
					element->LinkEndChild(blendHeight);

					blendHeight = document->NewElement("blendHeight4");
					blendHeight->SetText(data->blendHeight.w);
					element->LinkEndChild(blendHeight);

					element = document->NewElement("BlendUse");
					node->LinkEndChild(element);

					Xml::XMLElement * useBlend = NULL;
					useBlend = document->NewElement("UseBlend1");
					useBlend->SetText(data->activeBlend.x);
					element->LinkEndChild(useBlend);

					useBlend = document->NewElement("UseBlend2");
					useBlend->SetText(data->activeBlend.y);
					element->LinkEndChild(useBlend);

					useBlend = document->NewElement("UseBlend3");
					useBlend->SetText(data->activeBlend.z);
					element->LinkEndChild(useBlend);

					//TerrainDetail정보 저장
					element = document->NewElement("DetailValue");
					element->SetText(data->DetailValue);
					node->LinkEndChild(element);

					element = document->NewElement("DetailIntensity");
					element->SetText(data->DetailIntensity);
					node->LinkEndChild(element);

					element = document->NewElement("AmbientColor");
					node->LinkEndChild(element);

					Xml::XMLElement * ambientColor = NULL;
					ambientColor = document->NewElement("AmbientR");
					ambientColor->SetText(data->ambient.r);
					element->LinkEndChild(ambientColor);

					ambientColor = document->NewElement("AmbientG");
					ambientColor->SetText(data->ambient.g);
					element->LinkEndChild(ambientColor);

					ambientColor = document->NewElement("AmbientB");
					ambientColor->SetText(data->ambient.b);
					element->LinkEndChild(ambientColor);

					ambientColor = document->NewElement("AmbientA");
					ambientColor->SetText(data->ambient.a);
					element->LinkEndChild(ambientColor);

					//터레인 디퓨즈,스펙큘러,노말,디테일 복사
					element = document->NewElement("TerrainDiffuse");
					element->SetText(String::ToString(data->diffuseMap).c_str());

					node->LinkEndChild(element);

					element = document->NewElement("DiffuseColor");
					Xml::XMLElement * diffuseColor = NULL;
					diffuseColor = document->NewElement("DiffuseR");
					diffuseColor->SetText(data->diffuse.r);
					element->LinkEndChild(diffuseColor);

					diffuseColor = document->NewElement("DiffuseG");
					diffuseColor->SetText(data->diffuse.g);
					element->LinkEndChild(diffuseColor);

					diffuseColor = document->NewElement("DiffuseB");
					diffuseColor->SetText(data->diffuse.b);
					element->LinkEndChild(diffuseColor);

					diffuseColor = document->NewElement("DiffuseA");
					diffuseColor->SetText(data->diffuse.a);
					element->LinkEndChild(diffuseColor);
					node->LinkEndChild(element);

					element = document->NewElement("TerrainSpecular");
					element->SetText(String::ToString(data->specularMap).c_str());

					node->LinkEndChild(element);

					element = document->NewElement("SpecularColor");

					Xml::XMLElement * specularColor = NULL;
					specularColor = document->NewElement("SpecularR");
					specularColor->SetText(data->specular.r);
					element->LinkEndChild(specularColor);

					specularColor = document->NewElement("SpecularG");
					specularColor->SetText(data->specular.g);
					element->LinkEndChild(specularColor);

					specularColor = document->NewElement("SpecularB");
					specularColor->SetText(data->specular.b);
					element->LinkEndChild(specularColor);

					specularColor = document->NewElement("SpecularA");
					specularColor->SetText(data->specular.a);
					element->LinkEndChild(specularColor);
					node->LinkEndChild(element);

					element = document->NewElement("SpecularIntensity");
					element->SetText(data->intensity);
					node->LinkEndChild(element);

					element = document->NewElement("TerrainNormal");
					element->SetText(String::ToString(data->normalMap).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("TerrainDetail");
					element->SetText(String::ToString(data->detailMap).c_str());
					node->LinkEndChild(element);

					//터레인렌더 버퍼저장
					element = document->NewElement("TexScale");
					element->SetText(data->texScale);
					node->LinkEndChild(element);

					element = document->NewElement("FogStart");
					element->SetText(data->fogStart);
					node->LinkEndChild(element);

					element = document->NewElement("FogRange");
					element->SetText(data->fogRange);
					node->LinkEndChild(element);

					element = document->NewElement("StartBillboard");
					element->SetText("StartBillboard");
					node->LinkEndChild(element);
					//빌보드저장
					//만약넘어온 빌보드벡터사이즈가 0이면 저장하지 않는다


					element = document->NewElement("VectorSize");
					element->SetText(data->billboards.size());
					node->LinkEndChild(element);

					element = document->NewElement("Billboards");
					node->LinkEndChild(element);

					Xml::XMLElement * billElement = NULL;
					for (size_t i = 0; i < data->billboards.size(); i++)
					{
						string name = "bill" + to_string(i);
						billElement = document->NewElement(name.c_str());
						element->LinkEndChild(billElement);

						Xml::XMLElement * billinfo = NULL;

						billinfo = document->NewElement("PositionX");
						billinfo->SetText(data->billboards[i].position.x);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("PositionY");
						billinfo->SetText(data->billboards[i].position.y);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("PositionZ");
						billinfo->SetText(data->billboards[i].position.z);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("RotateX");
						billinfo->SetText(data->billboards[i].rotate.x);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("RotateY");
						billinfo->SetText(data->billboards[i].rotate.y);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("RotateZ");
						billinfo->SetText(data->billboards[i].rotate.z);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("SizeX");
						billinfo->SetText(data->billboards[i].size.x);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("SizeY");
						billinfo->SetText(data->billboards[i].size.y);
						billElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("textureNum");
						billinfo->SetText(data->billboards[i].textureNum);
						billElement->LinkEndChild(billinfo);
					}
					if (billElement != NULL)
						element->LinkEndChild(billElement);

					element = document->NewElement("StartBillboardCollider");
					element->SetText("BillboardCollider");
					node->LinkEndChild(element);

					element = document->NewElement("BillboardVectorSize");
					element->SetText(data->billboardCollider.size());
					node->LinkEndChild(element);

					element = document->NewElement("BillboardCollider");
					node->LinkEndChild(element);

					Xml::XMLElement * colliderElement = NULL;
					for (size_t i = 0; i < data->billboardCollider.size(); i++)
					{
						string name = "collider" + to_string(i);
						colliderElement = document->NewElement(name.c_str());
						element->LinkEndChild(colliderElement);

						Xml::XMLElement * billinfo = NULL;

						billinfo = document->NewElement("PositionX");
						billinfo->SetText(data->billboardCollider[i].position.x);
						colliderElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("PositionY");
						billinfo->SetText(data->billboardCollider[i].position.y);
						colliderElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("PositionZ");
						billinfo->SetText(data->billboardCollider[i].position.z);
						colliderElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("SizeX");
						billinfo->SetText(data->billboardCollider[i].scale.x);
						colliderElement->LinkEndChild(billinfo);

						billinfo = document->NewElement("SizeY");
						billinfo->SetText(data->billboardCollider[i].scale.y);
						colliderElement->LinkEndChild(billinfo);
					}
					if (colliderElement != NULL)
						element->LinkEndChild(colliderElement);

					node->LinkEndChild(element);

					string file = String::ToString(Datas + fileName + L"/Terrain0.terrain");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Rain:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					RainData*data = (RainData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("RainDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Name");
					element->SetText("Rain");
					node->LinkEndChild(element);

					element = document->NewElement("RainSize");

					Xml::XMLElement * rainSize = NULL;
					rainSize = document->NewElement("RainSizeX");
					rainSize->SetText(data->size.x);
					element->LinkEndChild(rainSize);

					rainSize = document->NewElement("RainSizeY");
					rainSize->SetText(data->size.y);
					element->LinkEndChild(rainSize);

					rainSize = document->NewElement("RainSizeZ");
					rainSize->SetText(data->size.z);
					element->LinkEndChild(rainSize);
					node->LinkEndChild(element);

					element = document->NewElement("Velocity");

					Xml::XMLElement * rainVelocity = NULL;
					rainVelocity = document->NewElement("VelocityX");
					rainVelocity->SetText(data->velocity.x);
					element->LinkEndChild(rainVelocity);

					rainVelocity = document->NewElement("VelocityY");
					rainVelocity->SetText(data->velocity.y);
					element->LinkEndChild(rainVelocity);

					rainVelocity = document->NewElement("VelocityZ");
					rainVelocity->SetText(data->velocity.z);
					element->LinkEndChild(rainVelocity);
					node->LinkEndChild(element);

					element = document->NewElement("DrawDist");
					element->SetText(data->dist);
					node->LinkEndChild(element);

					element = document->NewElement("RainColor");

					Xml::XMLElement * rainColor = NULL;
					rainColor = document->NewElement("ColorR");
					rainColor->SetText(data->color.r);
					element->LinkEndChild(rainColor);

					rainColor = document->NewElement("ColorG");
					rainColor->SetText(data->color.g);
					element->LinkEndChild(rainColor);

					rainColor = document->NewElement("ColorB");
					rainColor->SetText(data->color.b);
					element->LinkEndChild(rainColor);

					rainColor = document->NewElement("ColorA");
					rainColor->SetText(data->color.a);
					element->LinkEndChild(rainColor);
					node->LinkEndChild(element);

					element = document->NewElement("RainCount");
					element->SetText(data->particleCount);
					node->LinkEndChild(element);

					string file = String::ToString(Datas + fileName + L"/Rain0.rain");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Model:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					ModelData*data = (ModelData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("AnimDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Type");
					element->SetText(String::ToString(data->type).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText(String::ToString(data->meshFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MaterialFile");
					element->SetText(String::ToString(data->materialFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Name");
					element->SetText(String::ToString(data->modelName).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MinX");
					element->SetText(data->min.x);
					node->LinkEndChild(element);

					element = document->NewElement("MinY");
					element->SetText(data->min.y);
					node->LinkEndChild(element);

					element = document->NewElement("MinZ");
					element->SetText(data->min.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaxX");
					element->SetText(data->max.x);
					node->LinkEndChild(element);

					element = document->NewElement("MaxY");
					element->SetText(data->max.y);
					node->LinkEndChild(element);

					element = document->NewElement("MaxZ");
					element->SetText(data->max.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaterialSize");
					element->SetText(data->materials.size());
					node->LinkEndChild(element);

					//material
					for (size_t i = 0; i < data->materials.size(); i++)
					{
						string name = "material" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement* materialElement = NULL;

						materialElement = document->NewElement("DiffuseFile");
						wstring diffuseMap;
						if (data->materials[i]->GetDiffuseMap() != NULL)
							diffuseMap = data->materials[i]->GetDiffuseMap()->GetFile();
						else
							diffuseMap = L"";

						materialElement->SetText(String::ToString(diffuseMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("SpecularFile");
						wstring specularMap;
						if (data->materials[i]->GetSpecularMap() != NULL)
							specularMap = data->materials[i]->GetSpecularMap()->GetFile();
						else
							specularMap = L"";
						materialElement->SetText(String::ToString(specularMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("NormalFile");
						wstring normalMap;
						if (data->materials[i]->GetNormalMap() != NULL)
							normalMap = data->materials[i]->GetNormalMap()->GetFile();
						else
							normalMap = L"";
						materialElement->SetText(String::ToString(normalMap).c_str());
						element->LinkEndChild(materialElement);


						//diffuse Color
						{
							materialElement = document->NewElement("Diffuse");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * diffuseColor = NULL;

							diffuseColor = document->NewElement("R");
							diffuseColor->SetText(data->materials[i]->GetDiffuse().r);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("G");
							diffuseColor->SetText(data->materials[i]->GetDiffuse().g);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("B");
							diffuseColor->SetText(data->materials[i]->GetDiffuse().b);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("A");
							diffuseColor->SetText(data->materials[i]->GetDiffuse().a);
							materialElement->LinkEndChild(diffuseColor);
						}

						//specular Color
						{
							materialElement = document->NewElement("Specular");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * specularColor = NULL;

							specularColor = document->NewElement("R");
							specularColor->SetText(data->materials[i]->GetSpecular().r);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("G");
							specularColor->SetText(data->materials[i]->GetSpecular().g);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("B");
							specularColor->SetText(data->materials[i]->GetSpecular().b);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("A");
							specularColor->SetText(data->materials[i]->GetSpecular().a);
							materialElement->LinkEndChild(specularColor);
						}
					}

					element = document->NewElement("ColliderSize");
					element->SetText(data->colliders.size());
					node->LinkEndChild(element);

					//collider
					for (size_t i = 0; i < data->colliders.size(); i++)
					{
						string name = "collider" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * colliderElement = NULL;
						UINT type = data->colliders[i]->Type();

						colliderElement = document->NewElement("Type");

						if (type == 0)
							colliderElement->SetText("Box");
						else if (type == 1)
							colliderElement->SetText("Sphere");
						else if (type == 2)
							colliderElement->SetText("Ray");
						element->LinkEndChild(colliderElement);

						//콜라이더 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->colliders[i]->Position(&colPosition);

						colliderElement = document->NewElement("ColliderPosX");
						colliderElement->SetText(colPosition.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosY");
						colliderElement->SetText(colPosition.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosZ");
						colliderElement->SetText(colPosition.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colScale;
						data->colliders[i]->Scale(&colScale);

						colliderElement = document->NewElement("ColliderScaleX");
						colliderElement->SetText(colScale.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleY");
						colliderElement->SetText(colScale.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleZ");
						colliderElement->SetText(colScale.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colRotate;
						data->colliders[i]->Rotation(&colRotate);

						colliderElement = document->NewElement("ColliderRotateX");
						colliderElement->SetText(colRotate.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateY");
						colliderElement->SetText(colRotate.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateZ");
						colliderElement->SetText(colRotate.z);
						element->LinkEndChild(colliderElement);

						if (type == 1)
						{
							ColliderSphere*sphere = dynamic_cast<ColliderSphere*>(data->colliders[i]);
							colliderElement = document->NewElement("Radius");
							colliderElement->SetText(sphere->Radius());
							element->LinkEndChild(colliderElement);
						}
						else if (type == 2)
						{
							ColliderRay*ray = dynamic_cast<ColliderRay*>(data->colliders[i]);
							colliderElement = document->NewElement("Distance");
							colliderElement->SetText(ray->GetRay()->Distance);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionX");
							colliderElement->SetText(ray->GetRay()->Direction.x);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionY");
							colliderElement->SetText(ray->GetRay()->Direction.y);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionZ");
							colliderElement->SetText(ray->GetRay()->Direction.z);
							element->LinkEndChild(colliderElement);
						}
					}

					element = document->NewElement("TrailSize");
					element->SetText(data->trails.size());
					node->LinkEndChild(element);

					//trail
					for (size_t i = 0; i < data->trails.size(); i++)
					{
						string name = "Trail" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * trailElement = NULL;

						trailElement = document->NewElement("LifeTime");
						trailElement->SetText(data->trails[i]->GetLifeTime());
						element->LinkEndChild(trailElement);

						D3DXVECTOR3 startPos, endPos;
						startPos = { data->trails[i]->StartPos()._41,data->trails[i]->StartPos()._42,data->trails[i]->StartPos()._43 };
						endPos = { data->trails[i]->EndPos()._41,data->trails[i]->EndPos()._42,data->trails[i]->EndPos()._43 };

						trailElement = document->NewElement("StartPosX");
						trailElement->SetText(startPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosY");
						trailElement->SetText(startPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosZ");
						trailElement->SetText(startPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosX");
						trailElement->SetText(endPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosY");
						trailElement->SetText(endPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosZ");
						trailElement->SetText(endPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("DiffuseMap");
						trailElement->SetText(String::ToString(data->trails[i]->GetDiffuseMap()->GetFile()).c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("AlphaMap");
						trailElement->SetText(String::ToString(data->trails[i]->GetAlphaMap()->GetFile()).c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("BoneIndex");
						trailElement->SetText(data->trails[i]->GetBone());
						element->LinkEndChild(trailElement);
					}

					string modelName = String::ToString(data->modelName);


					int dotIndex = modelName.find(".");
					int nameSize = modelName.size();

					modelName.erase(dotIndex, nameSize);

					string file = String::ToString(Datas + fileName + L"/" + String::ToWString(modelName) + L".model");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Animation:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					AnimData*data = (AnimData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("ModelDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Type");
					element->SetText(String::ToString(data->modelData.type).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText(String::ToString(data->modelData.meshFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MaterialFile");
					element->SetText(String::ToString(data->modelData.materialFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Name");
					element->SetText(String::ToString(data->modelData.modelName).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MinX");
					element->SetText(data->modelData.min.x);
					node->LinkEndChild(element);

					element = document->NewElement("MinY");
					element->SetText(data->modelData.min.y);
					node->LinkEndChild(element);

					element = document->NewElement("MinZ");
					element->SetText(data->modelData.min.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaxX");
					element->SetText(data->modelData.max.x);
					node->LinkEndChild(element);

					element = document->NewElement("MaxY");
					element->SetText(data->modelData.max.y);
					node->LinkEndChild(element);

					element = document->NewElement("MaxZ");
					element->SetText(data->modelData.max.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaterialSize");
					element->SetText(data->modelData.materials.size());
					node->LinkEndChild(element);

					//material
					for (size_t i = 0; i < data->modelData.materials.size(); i++)
					{
						string name = "material" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement* materialElement = NULL;

						materialElement = document->NewElement("DiffuseFile");
						wstring diffuseMap;
						if (data->modelData.materials[i]->GetDiffuseMap() != NULL)
							diffuseMap = data->modelData.materials[i]->GetDiffuseMap()->GetFile();
						else
							diffuseMap = L"";

						materialElement->SetText(String::ToString(diffuseMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("SpecularFile");
						wstring specularMap;
						if (data->modelData.materials[i]->GetSpecularMap() != NULL)
							specularMap = data->modelData.materials[i]->GetSpecularMap()->GetFile();
						else
							specularMap = L"";
						materialElement->SetText(String::ToString(specularMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("NormalFile");
						wstring normalMap;
						if (data->modelData.materials[i]->GetNormalMap() != NULL)
							normalMap = data->modelData.materials[i]->GetNormalMap()->GetFile();
						else
							normalMap = L"";
						materialElement->SetText(String::ToString(normalMap).c_str());
						element->LinkEndChild(materialElement);


						//diffuse Color
						{
							materialElement = document->NewElement("Diffuse");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * diffuseColor = NULL;

							diffuseColor = document->NewElement("R");
							diffuseColor->SetText(data->modelData.materials[i]->GetDiffuse().r);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("G");
							diffuseColor->SetText(data->modelData.materials[i]->GetDiffuse().g);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("B");
							diffuseColor->SetText(data->modelData.materials[i]->GetDiffuse().b);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("A");
							diffuseColor->SetText(data->modelData.materials[i]->GetDiffuse().a);
							materialElement->LinkEndChild(diffuseColor);
						}

						//specular Color
						{
							materialElement = document->NewElement("Specular");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * specularColor = NULL;

							specularColor = document->NewElement("R");
							specularColor->SetText(data->modelData.materials[i]->GetSpecular().r);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("G");
							specularColor->SetText(data->modelData.materials[i]->GetSpecular().g);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("B");
							specularColor->SetText(data->modelData.materials[i]->GetSpecular().b);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("A");
							specularColor->SetText(data->modelData.materials[i]->GetSpecular().a);
							materialElement->LinkEndChild(specularColor);
						}
					}

					element = document->NewElement("ColliderSize");
					element->SetText(data->modelData.colliders.size());
					node->LinkEndChild(element);

					//collider
					for (size_t i = 0; i < data->modelData.colliders.size(); i++)
					{
						string name = "collider" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * colliderElement = NULL;
						UINT type = data->modelData.colliders[i]->Type();

						colliderElement = document->NewElement("Type");

						if (type == 0)
							colliderElement->SetText("Box");
						else if (type == 1)
							colliderElement->SetText("Sphere");
						else if (type == 2)
							colliderElement->SetText("Ray");
						element->LinkEndChild(colliderElement);

						//콜라이더 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->modelData.colliders[i]->Position(&colPosition);

						colliderElement = document->NewElement("ColliderPosX");
						colliderElement->SetText(colPosition.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosY");
						colliderElement->SetText(colPosition.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosZ");
						colliderElement->SetText(colPosition.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colScale;
						data->modelData.colliders[i]->Scale(&colScale);

						colliderElement = document->NewElement("ColliderScaleX");
						colliderElement->SetText(colScale.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleY");
						colliderElement->SetText(colScale.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleZ");
						colliderElement->SetText(colScale.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colRotate;
						data->modelData.colliders[i]->Rotation(&colRotate);

						colliderElement = document->NewElement("ColliderRotateX");
						colliderElement->SetText(colRotate.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateY");
						colliderElement->SetText(colRotate.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateZ");
						colliderElement->SetText(colRotate.z);
						element->LinkEndChild(colliderElement);

						if (type == 1)
						{
							ColliderSphere*sphere = dynamic_cast<ColliderSphere*>(data->modelData.colliders[i]);
							colliderElement = document->NewElement("Radius");
							colliderElement->SetText(sphere->Radius());
							element->LinkEndChild(colliderElement);
						}
						else if (type == 2)
						{
							ColliderRay*ray = dynamic_cast<ColliderRay*>(data->modelData.colliders[i]);
							colliderElement = document->NewElement("Distance");
							colliderElement->SetText(ray->GetRay()->Distance);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionX");
							colliderElement->SetText(ray->GetRay()->Direction.x);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionY");
							colliderElement->SetText(ray->GetRay()->Direction.y);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionZ");
							colliderElement->SetText(ray->GetRay()->Direction.z);
							element->LinkEndChild(colliderElement);
						}
					}

					element = document->NewElement("TrailSize");
					element->SetText(data->modelData.trails.size());
					node->LinkEndChild(element);

					//trail
					for (size_t i = 0; i < data->modelData.trails.size(); i++)
					{
						string name = "Trail" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * trailElement = NULL;
						UINT type = data->modelData.colliders[i]->Type();

						trailElement = document->NewElement("LifeTime");
						trailElement->SetText(data->modelData.trails[i]->GetLifeTime());
						element->LinkEndChild(trailElement);

						D3DXVECTOR3 startPos, endPos;
						startPos = { data->modelData.trails[i]->StartPos()._41,data->modelData.trails[i]->StartPos()._42,data->modelData.trails[i]->StartPos()._43 };
						endPos = { data->modelData.trails[i]->EndPos()._41,data->modelData.trails[i]->EndPos()._42,data->modelData.trails[i]->EndPos()._43 };

						trailElement = document->NewElement("StartPosX");
						trailElement->SetText(startPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosY");
						trailElement->SetText(startPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosZ");
						trailElement->SetText(startPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosX");
						trailElement->SetText(endPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosY");
						trailElement->SetText(endPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosZ");
						trailElement->SetText(endPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("DiffuseMap");
						trailElement->SetText(data->modelData.trails[i]->GetDiffuseMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("AlphaMap");
						trailElement->SetText(data->modelData.trails[i]->GetAlphaMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("BoneIndex");
						trailElement->SetText(data->modelData.trails[i]->GetBone());
						element->LinkEndChild(trailElement);
					}


					element = document->NewElement("ClipStart");
					element->SetText("Clip");
					node->LinkEndChild(element);

					element = document->NewElement("ClipSize");
					element->SetText(data->clips.size());
					node->LinkEndChild(element);

					for (UINT i = 0; i < data->clips.size(); i++)
					{
						string name = String::ToString(data->clips[i]->FileName());

						name = Path::GetFileName(name);
						int dotIndex = name.find(".");
						int nameSize = name.size();
						name.erase(dotIndex, nameSize);

						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * clipElement = NULL;

						name = String::ToString(data->clips[i]->FileName());
						clipElement = document->NewElement("FileName");
						clipElement->SetText(name.c_str());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Speed");
						clipElement->SetText(data->clips[i]->Speed());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("StartFrame");
						clipElement->SetText(data->clips[i]->StartFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("EndtFrame");
						clipElement->SetText(data->clips[i]->EndFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Repeat");
						clipElement->SetText(data->clips[i]->IsRepeat());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("BlendTime");
						clipElement->SetText(data->clips[i]->BlendTime());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("InPlace");
						clipElement->SetText(data->clips[i]->IsInPlace());
						element->LinkEndChild(clipElement);
					}

					string modelName = String::ToString(data->modelData.modelName);

					int dotIndex = modelName.find(".");
					int nameSize = modelName.size();

					modelName.erase(dotIndex, nameSize);

					string file = String::ToString(Datas + fileName + L"/" + String::ToWString(modelName) + L".charactor");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Player:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					PlayerData*data = (PlayerData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("ModelDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Type");
					element->SetText(String::ToString(data->animData.modelData.type).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText(String::ToString(data->animData.modelData.meshFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MaterialFile");
					element->SetText(String::ToString(data->animData.modelData.materialFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Name");
					element->SetText(String::ToString(data->animData.modelData.modelName).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MinX");
					element->SetText(data->animData.modelData.min.x);
					node->LinkEndChild(element);

					element = document->NewElement("MinY");
					element->SetText(data->animData.modelData.min.y);
					node->LinkEndChild(element);

					element = document->NewElement("MinZ");
					element->SetText(data->animData.modelData.min.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaxX");
					element->SetText(data->animData.modelData.max.x);
					node->LinkEndChild(element);

					element = document->NewElement("MaxY");
					element->SetText(data->animData.modelData.max.y);
					node->LinkEndChild(element);

					element = document->NewElement("MaxZ");
					element->SetText(data->animData.modelData.max.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaterialSize");
					element->SetText(data->animData.modelData.materials.size());
					node->LinkEndChild(element);

					//material
					for (size_t i = 0; i < data->animData.modelData.materials.size(); i++)
					{
						string name = "material" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement* materialElement = NULL;

						materialElement = document->NewElement("DiffuseFile");
						wstring diffuseMap;
						if (data->animData.modelData.materials[i]->GetDiffuseMap() != NULL)
							diffuseMap = data->animData.modelData.materials[i]->GetDiffuseMap()->GetFile();
						else
							diffuseMap = L"";

						materialElement->SetText(String::ToString(diffuseMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("SpecularFile");
						wstring specularMap;
						if (data->animData.modelData.materials[i]->GetSpecularMap() != NULL)
							specularMap = data->animData.modelData.materials[i]->GetSpecularMap()->GetFile();
						else
							specularMap = L"";
						materialElement->SetText(String::ToString(specularMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("NormalFile");
						wstring normalMap;
						if (data->animData.modelData.materials[i]->GetNormalMap() != NULL)
							normalMap = data->animData.modelData.materials[i]->GetNormalMap()->GetFile();
						else
							normalMap = L"";
						materialElement->SetText(String::ToString(normalMap).c_str());
						element->LinkEndChild(materialElement);


						//diffuse Color
						{
							materialElement = document->NewElement("Diffuse");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * diffuseColor = NULL;

							diffuseColor = document->NewElement("R");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().r);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("G");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().g);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("B");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().b);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("A");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().a);
							materialElement->LinkEndChild(diffuseColor);
						}

						//specular Color
						{
							materialElement = document->NewElement("Specular");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * specularColor = NULL;

							specularColor = document->NewElement("R");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().r);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("G");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().g);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("B");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().b);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("A");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().a);
							materialElement->LinkEndChild(specularColor);
						}
					}

					element = document->NewElement("ColliderSize");
					element->SetText(data->animData.modelData.colliders.size());
					node->LinkEndChild(element);

					//collider
					for (size_t i = 0; i < data->animData.modelData.colliders.size(); i++)
					{
						string name = "collider" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * colliderElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						colliderElement = document->NewElement("Type");

						if (type == 0)
							colliderElement->SetText("Box");
						else if (type == 1)
							colliderElement->SetText("Sphere");
						else if (type == 2)
							colliderElement->SetText("Ray");
						element->LinkEndChild(colliderElement);

						//콜라이더 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->animData.modelData.colliders[i]->Position(&colPosition);

						colliderElement = document->NewElement("ColliderPosX");
						colliderElement->SetText(colPosition.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosY");
						colliderElement->SetText(colPosition.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosZ");
						colliderElement->SetText(colPosition.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colScale;
						data->animData.modelData.colliders[i]->Scale(&colScale);

						colliderElement = document->NewElement("ColliderScaleX");
						colliderElement->SetText(colScale.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleY");
						colliderElement->SetText(colScale.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleZ");
						colliderElement->SetText(colScale.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colRotate;
						data->animData.modelData.colliders[i]->Rotation(&colRotate);

						colliderElement = document->NewElement("ColliderRotateX");
						colliderElement->SetText(colRotate.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateY");
						colliderElement->SetText(colRotate.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateZ");
						colliderElement->SetText(colRotate.z);
						element->LinkEndChild(colliderElement);

						if (type == 1)
						{
							ColliderSphere*sphere = dynamic_cast<ColliderSphere*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Radius");
							colliderElement->SetText(sphere->Radius());
							element->LinkEndChild(colliderElement);
						}
						else if (type == 2)
						{
							ColliderRay*ray = dynamic_cast<ColliderRay*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Distance");
							colliderElement->SetText(ray->GetRay()->Distance);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionX");
							colliderElement->SetText(ray->GetRay()->Direction.x);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionY");
							colliderElement->SetText(ray->GetRay()->Direction.y);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionZ");
							colliderElement->SetText(ray->GetRay()->Direction.z);
							element->LinkEndChild(colliderElement);
						}
					}

					element = document->NewElement("TrailSize");
					element->SetText(data->animData.modelData.trails.size());
					node->LinkEndChild(element);

					//trail
					for (size_t i = 0; i < data->animData.modelData.trails.size(); i++)
					{
						string name = "Trail" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * trailElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						trailElement = document->NewElement("LifeTime");
						trailElement->SetText(data->animData.modelData.trails[i]->GetLifeTime());
						element->LinkEndChild(trailElement);

						D3DXVECTOR3 startPos, endPos;
						startPos = { data->animData.modelData.trails[i]->StartPos()._41,data->animData.modelData.trails[i]->StartPos()._42,data->animData.modelData.trails[i]->StartPos()._43 };
						endPos = { data->animData.modelData.trails[i]->EndPos()._41,data->animData.modelData.trails[i]->EndPos()._42,data->animData.modelData.trails[i]->EndPos()._43 };

						trailElement = document->NewElement("StartPosX");
						trailElement->SetText(startPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosY");
						trailElement->SetText(startPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosZ");
						trailElement->SetText(startPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosX");
						trailElement->SetText(endPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosY");
						trailElement->SetText(endPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosZ");
						trailElement->SetText(endPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("DiffuseMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetDiffuseMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("AlphaMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetAlphaMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("BoneIndex");
						trailElement->SetText(data->animData.modelData.trails[i]->GetBone());
						element->LinkEndChild(trailElement);
					}

					element = document->NewElement("ClipStart");
					element->SetText("Clip");
					node->LinkEndChild(element);

					element = document->NewElement("ClipSize");
					element->SetText(data->animData.clips.size());
					node->LinkEndChild(element);

					for (UINT i = 0; i < data->animData.clips.size(); i++)
					{
						string name = String::ToString(data->animData.clips[i]->FileName());

						name = Path::GetFileName(name);
						int dotIndex = name.find(".");
						int nameSize = name.size();
						name.erase(dotIndex, nameSize);

						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * clipElement = NULL;

						name = String::ToString(data->animData.clips[i]->FileName());
						clipElement = document->NewElement("FileName");
						clipElement->SetText(name.c_str());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Speed");
						clipElement->SetText(data->animData.clips[i]->Speed());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("StartFrame");
						clipElement->SetText(data->animData.clips[i]->StartFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("EndtFrame");
						clipElement->SetText(data->animData.clips[i]->EndFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Repeat");
						clipElement->SetText(data->animData.clips[i]->IsRepeat());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("BlendTime");
						clipElement->SetText(data->animData.clips[i]->BlendTime());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("InPlace");
						clipElement->SetText(data->animData.clips[i]->IsInPlace());
						element->LinkEndChild(clipElement);
					}

					element = document->NewElement("WeaponStart");
					element->SetText("Weapon");
					node->LinkEndChild(element);

					element = document->NewElement("WeaponCount");
					element->SetText(data->weapons.size());
					node->LinkEndChild(element);

					for (UINT i = 0; i < data->weapons.size(); i++)
					{
						string name = String::ToString(data->weapons[i]->Name());
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * weaponNode = NULL;
						name = String::ToString(data->weapons[i]->MaterialFileName());
						weaponNode = document->NewElement("MaterialFile");
						weaponNode->SetText(name.c_str());
						element->LinkEndChild(weaponNode);

						name = String::ToString(data->weapons[i]->MeshFileName());
						weaponNode = document->NewElement("MeshFile");
						weaponNode->SetText(name.c_str());
						element->LinkEndChild(weaponNode);

						name = String::ToString(data->weapons[i]->Name());
						weaponNode = document->NewElement("Name");
						weaponNode->SetText(name.c_str());
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("BoneIndex");
						weaponNode->SetText(data->weapons[i]->TargetWorldBoneIndex());
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MinX");
						weaponNode->SetText(data->weapons[i]->GetModel()->Min().x);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MinY");
						weaponNode->SetText(data->weapons[i]->GetModel()->Min().y);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MinZ");
						weaponNode->SetText(data->weapons[i]->GetModel()->Min().z);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MaxX");
						weaponNode->SetText(data->weapons[i]->GetModel()->Max().x);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MaxY");
						weaponNode->SetText(data->weapons[i]->GetModel()->Max().y);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("MaxZ");
						weaponNode->SetText(data->weapons[i]->GetModel()->Max().z);
						element->LinkEndChild(weaponNode);

						//무기 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->weapons[i]->Position(&colPosition);

						weaponNode = document->NewElement("WeaponPosX");
						weaponNode->SetText(colPosition.x);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponPosY");
						weaponNode->SetText(colPosition.y);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponPosZ");
						weaponNode->SetText(colPosition.z);
						element->LinkEndChild(weaponNode);

						D3DXVECTOR3 colScale;
						data->weapons[i]->Scale(&colScale);

						weaponNode = document->NewElement("WeaponScaleX");
						weaponNode->SetText(colScale.x);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponScaleY");
						weaponNode->SetText(colScale.y);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponScaleZ");
						weaponNode->SetText(colScale.z);
						element->LinkEndChild(weaponNode);

						D3DXVECTOR3 colRotate;
						data->weapons[i]->Rotation(&colRotate);

						weaponNode = document->NewElement("WeaponRotateX");
						weaponNode->SetText(colRotate.x);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponRotateY");
						weaponNode->SetText(colRotate.y);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WeaponRotateZ");
						weaponNode->SetText(colRotate.z);
						element->LinkEndChild(weaponNode);

						weaponNode = document->NewElement("WColliderSize");
						weaponNode->SetText(data->weapons[i]->GetColliders().size());
						element->LinkEndChild(weaponNode);

						for (UINT j = 0; j < data->weapons[i]->GetColliders().size(); j++)
						{
							string name = String::ToString(data->weapons[i]->GetColliders()[j]->Name());
							weaponNode = document->NewElement(name.c_str());
							element->LinkEndChild(weaponNode);

							Xml::XMLElement * colElement = NULL;
							UINT type = data->weapons[i]->GetColliders()[j]->Type();

							colElement = document->NewElement("Type");

							if (type == 0)
								colElement->SetText("Box");
							else if (type == 1)
								colElement->SetText("Sphere");
							else if (type == 2)
								colElement->SetText("Ray");
							weaponNode->LinkEndChild(colElement);

							name = String::ToString(data->weapons[i]->GetColliders()[j]->Name());
							colElement = document->NewElement("Name");
							colElement->SetText(name.c_str());
							weaponNode->LinkEndChild(colElement);

							//콜라이더 자체 포지션 로테이션 스케일
							D3DXVECTOR3 colPosition;
							data->weapons[i]->GetColliders()[j]->Position(&colPosition);

							colElement = document->NewElement("ColliderPosX");
							colElement->SetText(colPosition.x);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderPosY");
							colElement->SetText(colPosition.y);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderPosZ");
							colElement->SetText(colPosition.z);
							weaponNode->LinkEndChild(colElement);

							D3DXVECTOR3 colScale;
							data->weapons[i]->GetColliders()[j]->Scale(&colScale);

							colElement = document->NewElement("ColliderScaleX");
							colElement->SetText(colScale.x);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderScaleY");
							colElement->SetText(colScale.y);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderScaleZ");
							colElement->SetText(colScale.z);
							weaponNode->LinkEndChild(colElement);

							D3DXVECTOR3 colRotate;
							data->weapons[i]->GetColliders()[j]->Rotation(&colRotate);

							colElement = document->NewElement("ColliderRotateX");
							colElement->SetText(colRotate.x);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderRotateY");
							colElement->SetText(colRotate.y);
							weaponNode->LinkEndChild(colElement);

							colElement = document->NewElement("ColliderRotateZ");
							colElement->SetText(colRotate.z);
							weaponNode->LinkEndChild(colElement);

							if (type == 2)
							{
								ColliderRay*ray = dynamic_cast<ColliderRay*>(data->weapons[i]->GetColliders()[j]);
								colElement = document->NewElement("Distance");
								colElement->SetText(ray->GetRay()->Distance);
								weaponNode->LinkEndChild(colElement);

								colElement = document->NewElement("DirectionX");
								colElement->SetText(ray->GetRay()->Direction.x);
								weaponNode->LinkEndChild(colElement);

								colElement = document->NewElement("DirectionY");
								colElement->SetText(ray->GetRay()->Direction.y);
								weaponNode->LinkEndChild(colElement);

								colElement = document->NewElement("DirectionZ");
								colElement->SetText(ray->GetRay()->Direction.z);
								weaponNode->LinkEndChild(colElement);
							}
						}
					}
					data->bt->SaveTree(node, element, document, data->bt->RootNode());

					string modelName = String::ToString(data->animData.modelData.modelName);

					int dotIndex = modelName.find(".");
					int nameSize = modelName.size();

					modelName.erase(dotIndex, nameSize);

					string file = String::ToString(Datas + fileName + L"/" + String::ToWString(modelName) + L".charactor");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Enemy1:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					EnemyData*data = (EnemyData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("ModelDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Type");
					element->SetText(String::ToString(data->animData.modelData.type).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText(String::ToString(data->animData.modelData.meshFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MaterialFile");
					element->SetText(String::ToString(data->animData.modelData.materialFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Name");
					element->SetText(String::ToString(data->animData.modelData.modelName).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MinX");
					element->SetText(data->animData.modelData.min.x);
					node->LinkEndChild(element);

					element = document->NewElement("MinY");
					element->SetText(data->animData.modelData.min.y);
					node->LinkEndChild(element);

					element = document->NewElement("MinZ");
					element->SetText(data->animData.modelData.min.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaxX");
					element->SetText(data->animData.modelData.max.x);
					node->LinkEndChild(element);

					element = document->NewElement("MaxY");
					element->SetText(data->animData.modelData.max.y);
					node->LinkEndChild(element);

					element = document->NewElement("MaxZ");
					element->SetText(data->animData.modelData.max.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaterialSize");
					element->SetText(data->animData.modelData.materials.size());
					node->LinkEndChild(element);

					//material
					for (size_t i = 0; i < data->animData.modelData.materials.size(); i++)
					{
						string name = "material" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement* materialElement = NULL;

						materialElement = document->NewElement("DiffuseFile");
						wstring diffuseMap;
						if (data->animData.modelData.materials[i]->GetDiffuseMap() != NULL)
							diffuseMap = data->animData.modelData.materials[i]->GetDiffuseMap()->GetFile();
						else
							diffuseMap = L"";

						materialElement->SetText(String::ToString(diffuseMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("SpecularFile");
						wstring specularMap;
						if (data->animData.modelData.materials[i]->GetSpecularMap() != NULL)
							specularMap = data->animData.modelData.materials[i]->GetSpecularMap()->GetFile();
						else
							specularMap = L"";
						materialElement->SetText(String::ToString(specularMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("NormalFile");
						wstring normalMap;
						if (data->animData.modelData.materials[i]->GetNormalMap() != NULL)
							normalMap = data->animData.modelData.materials[i]->GetNormalMap()->GetFile();
						else
							normalMap = L"";
						materialElement->SetText(String::ToString(normalMap).c_str());
						element->LinkEndChild(materialElement);


						//diffuse Color
						{
							materialElement = document->NewElement("Diffuse");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * diffuseColor = NULL;

							diffuseColor = document->NewElement("R");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().r);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("G");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().g);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("B");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().b);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("A");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().a);
							materialElement->LinkEndChild(diffuseColor);
						}

						//specular Color
						{
							materialElement = document->NewElement("Specular");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * specularColor = NULL;

							specularColor = document->NewElement("R");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().r);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("G");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().g);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("B");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().b);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("A");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().a);
							materialElement->LinkEndChild(specularColor);
						}
					}

					element = document->NewElement("ColliderSize");
					element->SetText(data->animData.modelData.colliders.size());
					node->LinkEndChild(element);

					//collider
					for (size_t i = 0; i < data->animData.modelData.colliders.size(); i++)
					{
						string name = "collider" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * colliderElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						colliderElement = document->NewElement("Type");

						if (type == 0)
							colliderElement->SetText("Box");
						else if (type == 1)
							colliderElement->SetText("Sphere");
						else if (type == 2)
							colliderElement->SetText("Ray");
						element->LinkEndChild(colliderElement);

						//콜라이더 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->animData.modelData.colliders[i]->Position(&colPosition);

						colliderElement = document->NewElement("ColliderPosX");
						colliderElement->SetText(colPosition.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosY");
						colliderElement->SetText(colPosition.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosZ");
						colliderElement->SetText(colPosition.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colScale;
						data->animData.modelData.colliders[i]->Scale(&colScale);

						colliderElement = document->NewElement("ColliderScaleX");
						colliderElement->SetText(colScale.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleY");
						colliderElement->SetText(colScale.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleZ");
						colliderElement->SetText(colScale.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colRotate;
						data->animData.modelData.colliders[i]->Rotation(&colRotate);

						colliderElement = document->NewElement("ColliderRotateX");
						colliderElement->SetText(colRotate.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateY");
						colliderElement->SetText(colRotate.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateZ");
						colliderElement->SetText(colRotate.z);
						element->LinkEndChild(colliderElement);

						if (type == 1)
						{
							ColliderSphere*sphere = dynamic_cast<ColliderSphere*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Radius");
							colliderElement->SetText(sphere->Radius());
							element->LinkEndChild(colliderElement);
						}
						else if (type == 2)
						{
							ColliderRay*ray = dynamic_cast<ColliderRay*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Distance");
							colliderElement->SetText(ray->GetRay()->Distance);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionX");
							colliderElement->SetText(ray->GetRay()->Direction.x);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionY");
							colliderElement->SetText(ray->GetRay()->Direction.y);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionZ");
							colliderElement->SetText(ray->GetRay()->Direction.z);
							element->LinkEndChild(colliderElement);
						}
					}

					element = document->NewElement("TrailSize");
					element->SetText(data->animData.modelData.trails.size());
					node->LinkEndChild(element);

					//trail
					for (size_t i = 0; i < data->animData.modelData.trails.size(); i++)
					{
						string name = "Trail" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * trailElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						trailElement = document->NewElement("LifeTime");
						trailElement->SetText(data->animData.modelData.trails[i]->GetLifeTime());
						element->LinkEndChild(trailElement);

						D3DXVECTOR3 startPos, endPos;
						startPos = { data->animData.modelData.trails[i]->StartPos()._41,data->animData.modelData.trails[i]->StartPos()._42,data->animData.modelData.trails[i]->StartPos()._43 };
						endPos = { data->animData.modelData.trails[i]->EndPos()._41,data->animData.modelData.trails[i]->EndPos()._42,data->animData.modelData.trails[i]->EndPos()._43 };

						trailElement = document->NewElement("StartPosX");
						trailElement->SetText(startPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosY");
						trailElement->SetText(startPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosZ");
						trailElement->SetText(startPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosX");
						trailElement->SetText(endPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosY");
						trailElement->SetText(endPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosZ");
						trailElement->SetText(endPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("DiffuseMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetDiffuseMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("AlphaMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetAlphaMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("BoneIndex");
						trailElement->SetText(data->animData.modelData.trails[i]->GetBone());
						element->LinkEndChild(trailElement);
					}

					element = document->NewElement("ClipStart");
					element->SetText("Clip");
					node->LinkEndChild(element);

					element = document->NewElement("ClipSize");
					element->SetText(data->animData.clips.size());
					node->LinkEndChild(element);

					for (UINT i = 0; i < data->animData.clips.size(); i++)
					{
						string name = String::ToString(data->animData.clips[i]->FileName());

						name = Path::GetFileName(name);
						int dotIndex = name.find(".");
						int nameSize = name.size();
						name.erase(dotIndex, nameSize);

						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * clipElement = NULL;

						name = String::ToString(data->animData.clips[i]->FileName());
						clipElement = document->NewElement("FileName");
						clipElement->SetText(name.c_str());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Speed");
						clipElement->SetText(data->animData.clips[i]->Speed());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("StartFrame");
						clipElement->SetText(data->animData.clips[i]->StartFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("EndtFrame");
						clipElement->SetText(data->animData.clips[i]->EndFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Repeat");
						clipElement->SetText(data->animData.clips[i]->IsRepeat());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("BlendTime");
						clipElement->SetText(data->animData.clips[i]->BlendTime());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("InPlace");
						clipElement->SetText(data->animData.clips[i]->IsInPlace());
						element->LinkEndChild(clipElement);
					}
					element = document->NewElement("BehaviorStart");
					element->SetText("BehaviorStart");
					node->LinkEndChild(element);
					data->bt->SaveTree(node, element, document, data->bt->RootNode());

					string modelName = String::ToString(data->animData.modelData.modelName);

					int dotIndex = modelName.find(".");
					int nameSize = modelName.size();

					modelName.erase(dotIndex, nameSize);

					string file = String::ToString(Datas + fileName + L"/" + String::ToWString(modelName) + L".charactor");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
			case FileType::Boss:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					EnemyData*data = (EnemyData*)diter->second[t]->Save();

					//Desc의 정보들 전부 저장
					Xml::XMLDocument*document = new Xml::XMLDocument();
					Xml::XMLDeclaration * decl = document->NewDeclaration();
					document->LinkEndChild(decl);

					Xml::XMLElement * root = document->NewElement("ModelDesc");
					root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
					root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
					document->LinkEndChild(root);

					Xml::XMLElement*node = document->NewElement("Desc");
					root->LinkEndChild(node);

					Xml::XMLElement * element = NULL;

					element = document->NewElement("Type");
					element->SetText(String::ToString(data->animData.modelData.type).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText(String::ToString(data->animData.modelData.meshFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MaterialFile");
					element->SetText(String::ToString(data->animData.modelData.materialFile).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("Name");
					element->SetText(String::ToString(data->animData.modelData.modelName).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MinX");
					element->SetText(data->animData.modelData.min.x);
					node->LinkEndChild(element);

					element = document->NewElement("MinY");
					element->SetText(data->animData.modelData.min.y);
					node->LinkEndChild(element);

					element = document->NewElement("MinZ");
					element->SetText(data->animData.modelData.min.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaxX");
					element->SetText(data->animData.modelData.max.x);
					node->LinkEndChild(element);

					element = document->NewElement("MaxY");
					element->SetText(data->animData.modelData.max.y);
					node->LinkEndChild(element);

					element = document->NewElement("MaxZ");
					element->SetText(data->animData.modelData.max.z);
					node->LinkEndChild(element);

					element = document->NewElement("MaterialSize");
					element->SetText(data->animData.modelData.materials.size());
					node->LinkEndChild(element);

					//material
					for (size_t i = 0; i < data->animData.modelData.materials.size(); i++)
					{
						string name = "material" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement* materialElement = NULL;

						materialElement = document->NewElement("DiffuseFile");
						wstring diffuseMap;
						if (data->animData.modelData.materials[i]->GetDiffuseMap() != NULL)
							diffuseMap = data->animData.modelData.materials[i]->GetDiffuseMap()->GetFile();
						else
							diffuseMap = L"";

						materialElement->SetText(String::ToString(diffuseMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("SpecularFile");
						wstring specularMap;
						if (data->animData.modelData.materials[i]->GetSpecularMap() != NULL)
							specularMap = data->animData.modelData.materials[i]->GetSpecularMap()->GetFile();
						else
							specularMap = L"";
						materialElement->SetText(String::ToString(specularMap).c_str());
						element->LinkEndChild(materialElement);

						materialElement = document->NewElement("NormalFile");
						wstring normalMap;
						if (data->animData.modelData.materials[i]->GetNormalMap() != NULL)
							normalMap = data->animData.modelData.materials[i]->GetNormalMap()->GetFile();
						else
							normalMap = L"";
						materialElement->SetText(String::ToString(normalMap).c_str());
						element->LinkEndChild(materialElement);


						//diffuse Color
						{
							materialElement = document->NewElement("Diffuse");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * diffuseColor = NULL;

							diffuseColor = document->NewElement("R");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().r);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("G");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().g);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("B");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().b);
							materialElement->LinkEndChild(diffuseColor);

							diffuseColor = document->NewElement("A");
							diffuseColor->SetText(data->animData.modelData.materials[i]->GetDiffuse().a);
							materialElement->LinkEndChild(diffuseColor);
						}

						//specular Color
						{
							materialElement = document->NewElement("Specular");
							element->LinkEndChild(materialElement);

							Xml::XMLElement * specularColor = NULL;

							specularColor = document->NewElement("R");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().r);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("G");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().g);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("B");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().b);
							materialElement->LinkEndChild(specularColor);

							specularColor = document->NewElement("A");
							specularColor->SetText(data->animData.modelData.materials[i]->GetSpecular().a);
							materialElement->LinkEndChild(specularColor);
						}
					}

					element = document->NewElement("ColliderSize");
					element->SetText(data->animData.modelData.colliders.size());
					node->LinkEndChild(element);

					//collider
					for (size_t i = 0; i < data->animData.modelData.colliders.size(); i++)
					{
						string name = "collider" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * colliderElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						colliderElement = document->NewElement("Type");

						if (type == 0)
							colliderElement->SetText("Box");
						else if (type == 1)
							colliderElement->SetText("Sphere");
						else if (type == 2)
							colliderElement->SetText("Ray");
						element->LinkEndChild(colliderElement);

						//콜라이더 자체 포지션 로테이션 스케일
						D3DXVECTOR3 colPosition;
						data->animData.modelData.colliders[i]->Position(&colPosition);

						colliderElement = document->NewElement("ColliderPosX");
						colliderElement->SetText(colPosition.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosY");
						colliderElement->SetText(colPosition.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderPosZ");
						colliderElement->SetText(colPosition.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colScale;
						data->animData.modelData.colliders[i]->Scale(&colScale);

						colliderElement = document->NewElement("ColliderScaleX");
						colliderElement->SetText(colScale.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleY");
						colliderElement->SetText(colScale.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderScaleZ");
						colliderElement->SetText(colScale.z);
						element->LinkEndChild(colliderElement);

						D3DXVECTOR3 colRotate;
						data->animData.modelData.colliders[i]->Rotation(&colRotate);

						colliderElement = document->NewElement("ColliderRotateX");
						colliderElement->SetText(colRotate.x);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateY");
						colliderElement->SetText(colRotate.y);
						element->LinkEndChild(colliderElement);

						colliderElement = document->NewElement("ColliderRotateZ");
						colliderElement->SetText(colRotate.z);
						element->LinkEndChild(colliderElement);

						if (type == 1)
						{
							ColliderSphere*sphere = dynamic_cast<ColliderSphere*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Radius");
							colliderElement->SetText(sphere->Radius());
							element->LinkEndChild(colliderElement);
						}
						else if (type == 2)
						{
							ColliderRay*ray = dynamic_cast<ColliderRay*>(data->animData.modelData.colliders[i]);
							colliderElement = document->NewElement("Distance");
							colliderElement->SetText(ray->GetRay()->Distance);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionX");
							colliderElement->SetText(ray->GetRay()->Direction.x);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionY");
							colliderElement->SetText(ray->GetRay()->Direction.y);
							element->LinkEndChild(colliderElement);

							colliderElement = document->NewElement("DirectionZ");
							colliderElement->SetText(ray->GetRay()->Direction.z);
							element->LinkEndChild(colliderElement);
						}
					}

					element = document->NewElement("TrailSize");
					element->SetText(data->animData.modelData.trails.size());
					node->LinkEndChild(element);

					//trail
					for (size_t i = 0; i < data->animData.modelData.trails.size(); i++)
					{
						string name = "Trail" + to_string(i);
						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * trailElement = NULL;
						UINT type = data->animData.modelData.colliders[i]->Type();

						trailElement = document->NewElement("LifeTime");
						trailElement->SetText(data->animData.modelData.trails[i]->GetLifeTime());
						element->LinkEndChild(trailElement);

						D3DXVECTOR3 startPos, endPos;
						startPos = { data->animData.modelData.trails[i]->StartPos()._41,data->animData.modelData.trails[i]->StartPos()._42,data->animData.modelData.trails[i]->StartPos()._43 };
						endPos = { data->animData.modelData.trails[i]->EndPos()._41,data->animData.modelData.trails[i]->EndPos()._42,data->animData.modelData.trails[i]->EndPos()._43 };

						trailElement = document->NewElement("StartPosX");
						trailElement->SetText(startPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosY");
						trailElement->SetText(startPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("StartPosZ");
						trailElement->SetText(startPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosX");
						trailElement->SetText(endPos.x);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosY");
						trailElement->SetText(endPos.y);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("EndPosZ");
						trailElement->SetText(endPos.z);
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("DiffuseMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetDiffuseMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("AlphaMap");
						trailElement->SetText(data->animData.modelData.trails[i]->GetAlphaMap()->GetFile().c_str());
						element->LinkEndChild(trailElement);

						trailElement = document->NewElement("BoneIndex");
						trailElement->SetText(data->animData.modelData.trails[i]->GetBone());
						element->LinkEndChild(trailElement);
					}

					element = document->NewElement("ClipStart");
					element->SetText("Clip");
					node->LinkEndChild(element);

					element = document->NewElement("ClipSize");
					element->SetText(data->animData.clips.size());
					node->LinkEndChild(element);

					for (UINT i = 0; i < data->animData.clips.size(); i++)
					{
						string name = String::ToString(data->animData.clips[i]->FileName());

						name = Path::GetFileName(name);
						int dotIndex = name.find(".");
						int nameSize = name.size();
						name.erase(dotIndex, nameSize);

						element = document->NewElement(name.c_str());
						node->LinkEndChild(element);

						Xml::XMLElement * clipElement = NULL;

						name = String::ToString(data->animData.clips[i]->FileName());
						clipElement = document->NewElement("FileName");
						clipElement->SetText(name.c_str());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Speed");
						clipElement->SetText(data->animData.clips[i]->Speed());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("StartFrame");
						clipElement->SetText(data->animData.clips[i]->StartFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("EndtFrame");
						clipElement->SetText(data->animData.clips[i]->EndFrame());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("Repeat");
						clipElement->SetText(data->animData.clips[i]->IsRepeat());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("BlendTime");
						clipElement->SetText(data->animData.clips[i]->BlendTime());
						element->LinkEndChild(clipElement);

						clipElement = document->NewElement("InPlace");
						clipElement->SetText(data->animData.clips[i]->IsInPlace());
						element->LinkEndChild(clipElement);
					}
					element = document->NewElement("BehaviorStart");
					element->SetText("BehaviorStart");
					node->LinkEndChild(element);
					data->bt->SaveTree(node, element, document, data->bt->RootNode());

					string modelName = String::ToString(data->animData.modelData.modelName);

					int dotIndex = modelName.find(".");
					int nameSize = modelName.size();

					modelName.erase(dotIndex, nameSize);

					string file = String::ToString(Datas + fileName + L"/" + String::ToWString(modelName) + L".charactor");
					document->SaveFile(file.c_str());
					SAFE_DELETE(data);
				}
			}
			break;
		}
	}
}

void FileManager::SaveScene(wstring fileName, UINT dataSize)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();
	Xml::XMLDeclaration * decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement * root = document->NewElement("SceneDesc");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	Xml::XMLElement*node = document->NewElement("Desc");
	root->LinkEndChild(node);

	Xml::XMLElement * element = NULL;

	element = document->NewElement("SceneName");
	element->SetText(String::ToString(fileName).c_str());
	node->LinkEndChild(element);

	element = document->NewElement("DataSize");
	element->SetText(dataSize);
	node->LinkEndChild(element);

	DataIter diter = dataMap.begin();

	for (; diter != dataMap.end(); diter++)
	{
		switch (diter->first)
		{
			case FileType::Sky:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					SkyData*data = (SkyData*)diter->second[t]->Save();
					
					element = document->NewElement("Name");
					element->SetText("Sky");
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->loadFileData).c_str());
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Terrain:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					TerrainData*data = (TerrainData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText("Terrain");
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->loadFileData).c_str());
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Rain:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					SkyData*data = (SkyData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText("Rain");
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->loadFileData).c_str());
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Model:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					ModelData*data = (ModelData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText((String::ToString(data->type)).c_str());
					node->LinkEndChild(element);

					GameModel*parentModel = dynamic_cast<GameModel*>(diter->second[t]);
					element = document->NewElement("MaterialFile");
					element->SetText((String::ToString(parentModel->MaterialFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText((String::ToString(parentModel->MeshFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->loadFileData).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("ScaleX");
					element->SetText(data->loadScale.x);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleY");
					element->SetText(data->loadScale.y);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleZ");
					element->SetText(data->loadScale.z);
					node->LinkEndChild(element);

					element = document->NewElement("RotateX");
					element->SetText(data->loadRotate.x);
					node->LinkEndChild(element);

					element = document->NewElement("RotateY");
					element->SetText(data->loadRotate.y);
					node->LinkEndChild(element);

					element = document->NewElement("RotateZ");
					element->SetText(data->loadRotate.z);
					node->LinkEndChild(element);

					element = document->NewElement("PosX");
					element->SetText(data->loadPosition.x);
					node->LinkEndChild(element);

					element = document->NewElement("PosY");
					element->SetText(data->loadPosition.y);
					node->LinkEndChild(element);

					element = document->NewElement("PosZ");
					element->SetText(data->loadPosition.z);
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Animation:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					AnimData*data = (AnimData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText((String::ToString(data->modelData.type)).c_str());
					node->LinkEndChild(element);

					GameModel*parentModel = dynamic_cast<GameModel*>(diter->second[t]);
					element = document->NewElement("MaterialFile");
					element->SetText((String::ToString(parentModel->MaterialFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText((String::ToString(parentModel->MeshFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->modelData.loadFileData).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("ScaleX");
					element->SetText(data->modelData.loadScale.x);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleY");
					element->SetText(data->modelData.loadScale.y);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleZ");
					element->SetText(data->modelData.loadScale.z);
					node->LinkEndChild(element);

					element = document->NewElement("RotateX");
					element->SetText(data->modelData.loadRotate.x);
					node->LinkEndChild(element);

					element = document->NewElement("RotateY");
					element->SetText(data->modelData.loadRotate.y);
					node->LinkEndChild(element);

					element = document->NewElement("RotateZ");
					element->SetText(data->modelData.loadRotate.z);
					node->LinkEndChild(element);

					element = document->NewElement("PosX");
					element->SetText(data->modelData.loadPosition.x);
					node->LinkEndChild(element);

					element = document->NewElement("PosY");
					element->SetText(data->modelData.loadPosition.y);
					node->LinkEndChild(element);

					element = document->NewElement("PosZ");
					element->SetText(data->modelData.loadPosition.z);
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Player:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					PlayerData*data = (PlayerData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText((String::ToString(data->animData.modelData.type)).c_str());
					node->LinkEndChild(element);

					GameModel*parentModel = dynamic_cast<GameModel*>(diter->second[t]);
					element = document->NewElement("MaterialFile");
					element->SetText((String::ToString(parentModel->MaterialFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText((String::ToString(parentModel->MeshFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->animData.modelData.loadFileData).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("ScaleX");
					element->SetText(data->animData.modelData.loadScale.x);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleY");
					element->SetText(data->animData.modelData.loadScale.y);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleZ");
					element->SetText(data->animData.modelData.loadScale.z);
					node->LinkEndChild(element);

					element = document->NewElement("RotateX");
					element->SetText(data->animData.modelData.loadRotate.x);
					node->LinkEndChild(element);

					element = document->NewElement("RotateY");
					element->SetText(data->animData.modelData.loadRotate.y);
					node->LinkEndChild(element);

					element = document->NewElement("RotateZ");
					element->SetText(data->animData.modelData.loadRotate.z);
					node->LinkEndChild(element);

					element = document->NewElement("PosX");
					element->SetText(data->animData.modelData.loadPosition.x);
					node->LinkEndChild(element);

					element = document->NewElement("PosY");
					element->SetText(data->animData.modelData.loadPosition.y);
					node->LinkEndChild(element);

					element = document->NewElement("PosZ");
					element->SetText(data->animData.modelData.loadPosition.z);
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Enemy1:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					EnemyData*data = (EnemyData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText("NearEnemy");
					node->LinkEndChild(element);

					GameModel*parentModel = dynamic_cast<GameModel*>(diter->second[t]);
					element = document->NewElement("MaterialFile");
					element->SetText((String::ToString(parentModel->MaterialFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText((String::ToString(parentModel->MeshFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->animData.modelData.loadFileData).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("ScaleX");
					element->SetText(data->animData.modelData.loadScale.x);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleY");
					element->SetText(data->animData.modelData.loadScale.y);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleZ");
					element->SetText(data->animData.modelData.loadScale.z);
					node->LinkEndChild(element);

					element = document->NewElement("RotateX");
					element->SetText(data->animData.modelData.loadRotate.x);
					node->LinkEndChild(element);

					element = document->NewElement("RotateY");
					element->SetText(data->animData.modelData.loadRotate.y);
					node->LinkEndChild(element);

					element = document->NewElement("RotateZ");
					element->SetText(data->animData.modelData.loadRotate.z);
					node->LinkEndChild(element);

					element = document->NewElement("PosX");
					element->SetText(data->animData.modelData.loadPosition.x);
					node->LinkEndChild(element);

					element = document->NewElement("PosY");
					element->SetText(data->animData.modelData.loadPosition.y);
					node->LinkEndChild(element);

					element = document->NewElement("PosZ");
					element->SetText(data->animData.modelData.loadPosition.z);
					node->LinkEndChild(element);
				}
			}
			break;
			case FileType::Boss:
			{
				for (size_t t = 0; t < diter->second.size(); t++)
				{
					EnemyData*data = (EnemyData*)diter->second[t]->Save();

					element = document->NewElement("Name");
					element->SetText("Boss");
					node->LinkEndChild(element);

					GameModel*parentModel = dynamic_cast<GameModel*>(diter->second[t]);
					element = document->NewElement("MaterialFile");
					element->SetText((String::ToString(parentModel->MaterialFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("MeshFile");
					element->SetText((String::ToString(parentModel->MeshFile())).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("LoadData");
					element->SetText(String::ToString(data->animData.modelData.loadFileData).c_str());
					node->LinkEndChild(element);

					element = document->NewElement("ScaleX");
					element->SetText(data->animData.modelData.loadScale.x);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleY");
					element->SetText(data->animData.modelData.loadScale.y);
					node->LinkEndChild(element);

					element = document->NewElement("ScaleZ");
					element->SetText(data->animData.modelData.loadScale.z);
					node->LinkEndChild(element);

					element = document->NewElement("RotateX");
					element->SetText(data->animData.modelData.loadRotate.x);
					node->LinkEndChild(element);

					element = document->NewElement("RotateY");
					element->SetText(data->animData.modelData.loadRotate.y);
					node->LinkEndChild(element);

					element = document->NewElement("RotateZ");
					element->SetText(data->animData.modelData.loadRotate.z);
					node->LinkEndChild(element);

					element = document->NewElement("PosX");
					element->SetText(data->animData.modelData.loadPosition.x);
					node->LinkEndChild(element);

					element = document->NewElement("PosY");
					element->SetText(data->animData.modelData.loadPosition.y);
					node->LinkEndChild(element);

					element = document->NewElement("PosZ");
					element->SetText(data->animData.modelData.loadPosition.z);
					node->LinkEndChild(element);
				}
			}
			break;
		}

		string file = String::ToString(Datas + L"/Scene" + fileName + L".scene");
		document->SaveFile(file.c_str());
	}
}

void FileManager::Load(wstring sceneName, bool modelsCopy, Effect*lineEffect)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = sceneName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement(); // SceneName
	wstring name = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // datasize
	UINT dataSize = node->IntText();

	for (UINT i = 0; i < dataSize; i++)
	{
		node = node->NextSiblingElement(); // name
		wstring name = String::ToWString(node->GetText());

		if (name == L"Terrain")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Terrain::InitDesc desc;
			Terrain*terrain = new Terrain(desc);

			terrain->Load(dataFile);

			terrain->Initialize();

			ObjectManager::Get()->AddTerrain(terrain);
		}
		else if (name == L"Rain")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Rain*rain = new Rain(D3DXVECTOR3(100, 100, 100), 1000);

			rain->Load(dataFile);

			ObjectManager::Get()->AddRain(rain);
		}
		else if (name == L"Sky")
		{
			node = node->NextSiblingElement(); // name
			wstring dataFile = String::ToWString(node->GetText());

			Sky*sky = new Sky();
			sky->Load(dataFile);
			sky->Initialize();

			ObjectManager::Get()->AddSky(sky);
		}
		else if (name == L"Model")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			DataIdxIter dIiter = dataIndexMap.find(meshFile);

			if (dIiter != dataIndexMap.end())
				dataIndexMap[meshFile]++;
			else
				dataIndexMap[meshFile] = 0;

			modelName.erase(dotIndex, nameSize);

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			GameModel*transModel = new GameModel(dataIndexMap[meshFile],materialFile,meshFile, lineEffect);

			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			ObjectManager::Get()->AddModel(String::ToWString(modelName), transModel);
		}
		else if (name == L"Animation")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			DataIdxIter dIiter = dataIndexMap.find(meshFile);

			if (dIiter != dataIndexMap.end())
				dataIndexMap[meshFile]++;
			else
				dataIndexMap[meshFile] = 0;

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			GameAnimator*transModel = new GameAnimator(dataIndexMap[meshFile], materialFile, meshFile, lineEffect);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			ObjectManager::Get()->AddModel(String::ToWString(modelName), transModel);
		}
		else if (name == L"Player")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			DataIdxIter dIiter = dataIndexMap.find(meshFile);

			if (dIiter != dataIndexMap.end())
				dataIndexMap[meshFile]++;
			else
				dataIndexMap[meshFile] = 0;

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			Player*transModel = new Player(dataIndexMap[meshFile], materialFile, meshFile, lineEffect);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel ->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			ObjectManager::Get()->AddModel(String::ToWString(modelName),transModel);
		}
		else if (name == L"NearEnemy")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			DataIdxIter dIiter = dataIndexMap.find(meshFile);

			if (dIiter != dataIndexMap.end())
				dataIndexMap[meshFile]++;
			else
				dataIndexMap[meshFile] = 0;

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			NearEnemy*transModel = new NearEnemy(dataIndexMap[meshFile], materialFile, meshFile, lineEffect);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			ObjectManager::Get()->AddModel(String::ToWString(modelName), (GameModel*)transModel);
		}
		else if (name == L"Boss")
		{
			node = node->NextSiblingElement(); // dataFile
			wstring materialFile = String::ToWString(node->GetText());

			node = node->NextSiblingElement(); // dataFile
			wstring meshFile = String::ToWString(node->GetText());

			string modelName = Path::GetFileName(String::ToString(meshFile));
			int dotIndex = modelName.find(".");
			int nameSize = modelName.size();

			modelName.erase(dotIndex, nameSize);

			DataIdxIter dIiter = dataIndexMap.find(meshFile);

			if (dIiter != dataIndexMap.end())
				dataIndexMap[meshFile]++;
			else
				dataIndexMap[meshFile] = 0;

			node = node->NextSiblingElement(); // dataFile
			wstring dataFile = String::ToWString(node->GetText());

			Boss*transModel = new Boss(dataIndexMap[meshFile], materialFile, meshFile, lineEffect);

			transModel->LoadStart() = true;
			transModel->Load(dataFile);

			D3DXVECTOR3 Scale, Rotate, Pos;
			node = node->NextSiblingElement(); // scalex
			Scale.x = node->FloatText();

			node = node->NextSiblingElement(); // scaley
			Scale.y = node->FloatText();

			node = node->NextSiblingElement(); // scalez
			Scale.z = node->FloatText();

			node = node->NextSiblingElement(); // rotatex
			Rotate.x = node->FloatText();

			node = node->NextSiblingElement(); // rotatey
			Rotate.y = node->FloatText();

			node = node->NextSiblingElement(); // rotatez
			Rotate.z = node->FloatText();

			node = node->NextSiblingElement(); // posx
			Pos.x = node->FloatText();

			node = node->NextSiblingElement(); // posy
			Pos.y = node->FloatText();

			node = node->NextSiblingElement(); // posz
			Pos.z = node->FloatText();

			transModel->LoadScale() = Scale;
			transModel->LoadRotate() = Rotate;
			transModel->LoadPosition() = Pos;

			transModel->LoadStart() = false;

			ObjectManager::Get()->AddModel(String::ToWString(modelName), (GameModel*)transModel);
		}
	}

	SAFE_DELETE(document);
}

void FileManager::SaveWithFolder(wstring folderName)
{
	Path::CreateFolder(Datas + folderName);

	Save(folderName);
}

ObjectManager*ObjectManager::instance = NULL;

void ObjectManager::Create()
{
	if (instance == NULL)
		instance = new ObjectManager();
}

void ObjectManager::Delete()
{
	FileManager::Get()->dataIndexMap.clear();
	//InstanceManager::Get()->RemoveAllModel();
	BulletManager::Get()->RemoveAll();
	ColliderManager::Get()->DeleteAll();

	if (instance != NULL)
	{
		mIter diter = instance->models.begin();
		for (; diter != instance->models.end(); diter++)
		{
			for (UINT i = 0; i < diter->second.size(); i++)
				SAFE_DELETE(diter->second[i]);
		}
	}

	SAFE_DELETE(instance->terrain);
	SAFE_DELETE(instance->rain);
	SAFE_DELETE(instance->sky);
	SAFE_DELETE(instance->shadow);

	instance->models.clear();

	//SAFE_DELETE(instance);
}

ObjectManager::ObjectManager()
{
	shadow = new Shadow(2048, 2048);
	shadow->Initialize();
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::AddModel(wstring modelName, GameModel * model)
{
	models[modelName].push_back(model);
}

void ObjectManager::AddTerrain(Terrain * terrain)
{
	this->terrain = terrain;
}

void ObjectManager::AddRain(Rain * rain)
{
	this->rain = rain;
}

void ObjectManager::AddSky(Sky * sky)
{
	this->sky = sky;
}

void ObjectManager::Update()
{
	if(terrain!=NULL)
		terrain->Update();
	if(rain !=NULL)
		rain->Update();
	if(sky!=NULL)
		sky->Update();
	if(shadow!=NULL)
		shadow->Update();

	for (mIter miter = models.begin(); miter != models.end(); miter++)
	{
		for (UINT i = 0; i < miter->second.size(); i++)
			miter->second[i]->Update();
	}
}

void ObjectManager::Ready()
{
	//===================플레이어 터레인셋팅====================//
	mIter miter = models.find(L"Player");

	if (miter != models.end())
	{
		if (terrain != NULL)
		{
			((Player*)miter->second[0])->GetTerrain(terrain);

			mIter miter2 = models.find(L"Boss");
			((Player*)miter->second[0])->GetBoss((Boss*)miter2->second[0]);
		}
	}
	//========================================================//


	//===================근접에너미 셋팅=======================//
	miter = models.find(L"NearEnemy");

	if (miter != models.end())
	{
		if (terrain != NULL)
		{
			for (size_t i = 0; i < miter->second.size(); i++)
			{
				((Enemy*)miter->second[i])->GetTerrain(terrain);
				((Enemy*)miter->second[i])->GetPlayer((Player*)models[L"Player"][0]);
			}
		}
	}
	//========================================================//

	//===================보스 셋팅=======================//
	miter = models.find(L"Boss");

	if (miter != models.end())
	{
		if (terrain != NULL)
		{
			for (size_t i = 0; i < miter->second.size(); i++)
			{
				((Enemy*)miter->second[i])->GetTerrain(terrain);
				((Enemy*)miter->second[i])->GetPlayer((Player*)models[L"Player"][0]);
			}
		}
	}
	//========================================================//


	for (miter = models.begin(); miter != models.end(); miter++)
	{
		for (UINT i = 0; i < miter->second.size(); i++)
			miter->second[i]->Ready();
	}

	if (terrain != NULL)
		terrain->Ready();
	if (rain != NULL)
		rain->Ready();
	if (sky != NULL)
		sky->Ready();
	if (shadow != NULL)
		shadow->Ready();

	if (terrain != NULL)
	{
		shadow->Add(terrain);
		shadow->Add(terrain->GetBillboard());
	}

	miter = models.begin();
	for (; miter != models.end(); miter++)
	{
		for (size_t i = 0; i < miter->second.size(); i++)
			shadow->Add(miter->second[i]);
	}
	
	miter = models.find(L"Player");

	if (miter != models.end())
	{
		if (terrain != NULL)
		{
			shadow->Add(((Player*)miter->second[0])->GetWeapons()[0]);
			shadow->Add(((Player*)miter->second[0])->GetWeapons()[1]);
		}
	}

	BulletManager::Get()->BulletInit(L"Bullet", L"Bullet.model", 10, 10, 1000);

	//==================플레이어 콜라이더셋팅===================//
	ColliderManager::Get()->InputObject(models[L"Player"][0]);
	ColliderManager::Get()->AddLayerToCollider(L"Collision Player", models[L"Player"][0]->GetColliders()[0]);
	ColliderManager::Get()->AddLayerToCollider(L"Collision PlayerSkill", models[L"Player"][0]->GetColliders()[1]);
	//========================================================//


	//=================근접에너미 콜라이더셋팅==================//
	miter = models.find(L"NearEnemy");
	if (miter != models.end())
	{
		for (size_t i = 0; i < miter->second.size(); i++)
		{
			ColliderManager::Get()->InputObject(miter->second[i]);
			
			for (size_t j = 0; j < miter->second[i]->GetColliders().size(); j++)
			{
				//첫번째 콜라이더는 에너미자체콜라이더
				//두번부터 콜라이더는 에너미공격콜라이더

				if (j == 0)
					ColliderManager::Get()->AddLayerToCollider(L"Collision Enemy", miter->second[i]->GetColliders()[j]);
				else
					ColliderManager::Get()->AddLayerToCollider(L"Collision EnemyAttack", miter->second[i]->GetColliders()[j]);
			}
		}
	}
    //========================================================//

	//=================보스 콜라이더셋팅==================//
	miter = models.find(L"Boss");
	if (miter != models.end())
	{
		for (size_t i = 0; i < miter->second.size(); i++)
		{
			ColliderManager::Get()->InputObject(miter->second[i]);

			for (size_t j = 0; j < miter->second[i]->GetColliders().size(); j++)
			{
				//첫번째 콜라이더는 에너미자체콜라이더
				//두번부터 콜라이더는 에너미공격콜라이더

				if (j == 0)
					ColliderManager::Get()->AddLayerToCollider(L"Collision Enemy", miter->second[i]->GetColliders()[j]);
				else if(j==1)
					ColliderManager::Get()->AddLayerToCollider(L"Collision EnemyAttack", miter->second[i]->GetColliders()[j]);
				else if(j==2)
					ColliderManager::Get()->AddLayerToCollider(L"Collision EnemySkill1", miter->second[i]->GetColliders()[j]);
				else if (j == 3)
					ColliderManager::Get()->AddLayerToCollider(L"Collision EnemySkill2", miter->second[i]->GetColliders()[j]);
			}
		}
	}
	//========================================================//


	ColliderManager::Get()->InputObject(terrain->GetBillboard());
	for (size_t i = 0; i < terrain->GetBillboard()->GetBillboardCollider().size(); i++)
		ColliderManager::Get()->AddLayerToCollider(L"Collision World", (ColliderElement*)terrain->GetBillboard()->GetBillboardCollider()[i].Box);
}

void ObjectManager::PreRender()
{
	if(sky!=NULL)
		sky->PreRender();
	if(shadow!=NULL)
		shadow->PreRender();
}

void ObjectManager::Render()
{
	if (sky != NULL)
		sky->Render();
	//InstanceManager::Get()->Render();
	if (shadow != NULL)
		shadow->Render();
	if (rain != NULL)
		rain->Render();

	for (mIter miter = models.begin(); miter != models.end(); miter++)
	{
		for (UINT i = 0; i < miter->second.size(); i++)
			miter->second[i]->Render();
	}
}

void ObjectManager::PostRender()
{
	
}

GameModel * ObjectManager::GetModel(wstring name,int idx)
{
	mIter miter = models.find(name);

	if (miter != models.end())
		return miter->second[idx];
	else
		return NULL;
}

vector<GameModel*>&ObjectManager::GetModelVector(wstring name)
{
	mIter miter = models.find(name);

	if (miter != models.end())
		return miter->second;

	return vector<GameModel*>();
}

GameModel * ObjectManager::GetPlayer()
{
	mIter miter = models.find(L"Player");
	
	if (miter != models.end())
		return miter->second[0];
	else
		return NULL;
}
