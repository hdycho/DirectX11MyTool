#include "Framework.h"
#include "Terrain.h"

#include "HeightMap.h"
#include "TerrainRender.h"
#include "Patch.h"
#include "Shadow.h"

#include "Systems\Window.h"
#include "Datas\GameData.h"
#include "Billboard.h"
#include "Collider\ColliderBox.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc & desc)
	: desc(desc)
	, isPick(false)
	, terrainName(L"")
	, render(NULL)
	, heightMap(NULL)
	, tree(NULL)
	, loadDataFile(L"")
{
	for (int i = 0; i < 4; i++)
		materialValueFile[i] = L"";

	this->desc.material = new Material(Effects + L"007_Terrain.hlsl");

	if (render == NULL)
		render = new TerrainRender(this);

	if (heightMap == NULL)
		heightMap = new HeightMap((UINT)desc.HeightMapWidth, (UINT)desc.HeightMapHeight, 80.0f);
	
	lineTypeVariable = this->desc.material->GetEffect()->AsScalar("LineType");
	spacingVariable = this->desc.material->GetEffect()->AsScalar("Spacing");
	thickVariable = this->desc.material->GetEffect()->AsScalar("Thickness");
	lineColorVariable = this->desc.material->GetEffect()->AsVector("LineColor");
}

Terrain::Terrain(Terrain & copy)
	:isPick(false)
	, terrainName(L"")
	, tree(NULL)
	, render(NULL)
	, heightMap(NULL)
	, loadDataFile(L"")
{

	for (int i = 0; i < 4; i++)
		materialValueFile[i] = L"";

	if (render == NULL)
		render = new TerrainRender(this);

	if (heightMap == NULL)
		heightMap = new HeightMap((UINT)desc.HeightMapWidth, (UINT)desc.HeightMapHeight, desc.HeightScale);

	terrainName = copy.terrainName;
	desc.HeightMap = copy.desc.HeightMap;
	desc.HeightScale = copy.desc.HeightScale;
	desc.HeightMapWidth = copy.desc.HeightMapWidth;
	desc.HeightMapHeight = copy.desc.HeightMapHeight;

	desc.layerMapFile[0] = copy.desc.layerMapFile[0];
	desc.layerMapFile[1] = copy.desc.layerMapFile[1];
	desc.layerMapFile[2] = copy.desc.layerMapFile[2];

	desc.blendHeight = copy.desc.blendHeight;
	desc.activeBlend = copy.desc.activeBlend;

	copy.desc.material->Clone((void**)&desc.material);

	detail.DetailValue = copy.detail.DetailValue;
	detail.DetailIntensity = copy.detail.DetailIntensity;

	render->GetBuffer().TexScale = copy.render->GetBuffer().TexScale;
	render->GetBuffer().FogStart = copy.render->GetBuffer().FogStart;
	render->GetBuffer().FogRange = copy.render->GetBuffer().FogRange;

	tree = new Billboard(*copy.tree);
	ColliderManager::Get()->InputObject(tree);
	tree->Initalize();
	
	lineTypeVariable = this->desc.material->GetEffect()->AsScalar("LineType");
	spacingVariable = this->desc.material->GetEffect()->AsScalar("Spacing");
	thickVariable = this->desc.material->GetEffect()->AsScalar("Thickness");
	lineColorVariable = this->desc.material->GetEffect()->AsVector("LineColor");

	loadFileName = copy.loadFileName;
}

Terrain::~Terrain()
{
	SAFE_DELETE(render);
	SAFE_DELETE(heightMap);
	SAFE_DELETE(desc.material);
	SAFE_DELETE(tree);
}

void Terrain::Initialize()
{
	heightMap->Load(desc.HeightMap);

	render->Initialize();
}

void Terrain::Ready()
{
	if (materialValueFile[0] != L"")
		desc.material->SetDiffuseMap(materialValueFile[0]);

	if (materialValueFile[1] != L"")
		desc.material->SetSpecularMap(materialValueFile[1]);

	if (materialValueFile[2] != L"")
		desc.material->SetNormalMap(materialValueFile[2]);

	if (materialValueFile[3] != L"")
		desc.material->SetDetailMap(materialValueFile[3]);

	render->Ready(desc.material);

	if (tree != NULL)
		tree->Ready();
}

void Terrain::Load(wstring terrainData)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = terrainData;
	loadFileName = terrainData;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement(); // Name
	wstring name = String::ToWString(node->GetText());

	node = node->NextSiblingElement();
	desc.HeightMap = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); //HeightScale
	desc.HeightScale = node->FloatText();

	node = node->NextSiblingElement(); // HeightMapWidth
	desc.HeightMapWidth = node->FloatText();

	node = node->NextSiblingElement(); // HeightMapHeight
	desc.HeightMapHeight = node->FloatText();

	node = node->NextSiblingElement(); // LayerMaps
	Xml::XMLElement*layerMap = node->FirstChildElement();
	desc.layerMapFile[0] = String::ToWString(layerMap->GetText());

	layerMap = layerMap->NextSiblingElement();
	desc.layerMapFile[1] = String::ToWString(layerMap->GetText());

	layerMap = layerMap->NextSiblingElement();
	desc.layerMapFile[2] = String::ToWString(layerMap->GetText());

	node = node->NextSiblingElement(); // BlendHeight
	Xml::XMLElement*blendHeight = node->FirstChildElement();
	desc.blendHeight.x = blendHeight->FloatText();

	blendHeight = blendHeight->NextSiblingElement();
	desc.blendHeight.y = blendHeight->FloatText();

	blendHeight = blendHeight->NextSiblingElement();
	desc.blendHeight.z = blendHeight->FloatText();

	blendHeight = blendHeight->NextSiblingElement();
	desc.blendHeight.w = blendHeight->FloatText();

	node = node->NextSiblingElement(); // BlendUse
	Xml::XMLElement*blendUse = node->FirstChildElement();
	desc.activeBlend.x = blendUse->FloatText();

	blendUse = blendUse->NextSiblingElement();
	desc.activeBlend.y = blendUse->FloatText();

	blendUse = blendUse->NextSiblingElement();
	desc.activeBlend.z = blendUse->FloatText();

	node = node->NextSiblingElement(); // DetailValue
	detail.DetailValue = node->FloatText();

	node = node->NextSiblingElement(); // DetailIntensity
	detail.DetailIntensity = node->FloatText();

	node = node->NextSiblingElement(); // AmbientColor
	Xml::XMLElement*ambientColor = node->FirstChildElement();
	D3DXCOLOR aColor;
	aColor.r = ambientColor->FloatText();

	ambientColor = ambientColor->NextSiblingElement();
	aColor.g = ambientColor->FloatText();

	ambientColor = ambientColor->NextSiblingElement();
	aColor.b = ambientColor->FloatText();

	ambientColor = ambientColor->NextSiblingElement();
	aColor.a = ambientColor->FloatText();

	desc.material->SetAmbient(aColor);

	node = node->NextSiblingElement(); // DiffuseMap
	materialValueFile[0] = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // DiffuseColor
	Xml::XMLElement*diffuseColor = node->FirstChildElement();

	D3DXCOLOR dColor;
	dColor.r = diffuseColor->FloatText();

	diffuseColor = diffuseColor->NextSiblingElement();
	dColor.g = diffuseColor->FloatText();

	diffuseColor = diffuseColor->NextSiblingElement();
	dColor.b = diffuseColor->FloatText();

	diffuseColor = diffuseColor->NextSiblingElement();
	dColor.a = diffuseColor->FloatText();

	desc.material->SetDiffuse(dColor);

	node = node->NextSiblingElement(); // SpecularMap
	materialValueFile[1] = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // SpecularColor
	Xml::XMLElement*specularColor = node->FirstChildElement();

	D3DXCOLOR sColor;
	sColor.r = specularColor->FloatText();

	specularColor = specularColor->NextSiblingElement();
	sColor.g = specularColor->FloatText();

	specularColor = specularColor->NextSiblingElement();
	sColor.b = specularColor->FloatText();

	specularColor = specularColor->NextSiblingElement();
	sColor.a = specularColor->FloatText();

	desc.material->SetSpecular(sColor);

	node = node->NextSiblingElement(); // SpecularIntensity
	
	D3DXCOLOR specular;
	specular = desc.material->GetSpecular();
	specular.a = node->FloatText();
	desc.material->SetSpecular(specular);

	node = node->NextSiblingElement(); // NormalMap
	materialValueFile[2] = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // DetailMap
	materialValueFile[3] = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // TexScale
	GetTerrainRender()->GetBuffer().TexScale = node->IntText();

	node = node->NextSiblingElement(); // FogStart
	GetTerrainRender()->GetBuffer().FogStart = node->FloatText();

	node = node->NextSiblingElement(); // FogRange
	GetTerrainRender()->GetBuffer().FogRange = node->FloatText();

	node = node->NextSiblingElement(); // StartBillboard
	node = node->NextSiblingElement(); // vectorsize

	if (node->FloatText() > 0)
	{
		tree = new Billboard(this);
		tree->Load(terrainData);
		tree->Initalize();
	}

	SAFE_DELETE(document);
}

void Terrain::SetShader(D3DXMATRIX v, D3DXMATRIX p)
{
	desc.material->LightView()->SetMatrix(v);
	desc.material->LightProjection()->SetMatrix(p);
}

void Terrain::SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv)
{
	desc.material->ShadowTransform()->SetMatrix(shadowTransform);
	desc.material->ShadowMap()->SetResource(srv);
}

void Terrain::ShadowUpdate()
{
}

void Terrain::NormalRender()
{
	render->Render();
}

void Terrain::ShadowRender(UINT tech, UINT pass)
{
	render->RenderShadow(tech, pass);
}

void Terrain::CreateBillboard(class Shadow*shadow, Effect*lineEffect)
{
	if (tree == NULL)
	{
		tree = new Billboard(this, lineEffect);
		tree->Initalize();
		tree->Ready();

		shadow->Add(tree);
		ColliderManager::Get()->InputObject(tree);
	}
}


void Terrain::Update()
{
	if (tree != NULL)
		tree->Update();
}

void Terrain::Render()
{
	render->Render();
	if (tree != NULL)
		tree->Render();
}


void Terrain::LineUpdate()
{
	lineTypeVariable->SetInt(tline.LineType);
	lineColorVariable->SetFloatVector(tline.LineColor);
	spacingVariable->SetInt(tline.Spacing);
	thickVariable->SetFloat(tline.Thickness);
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * Width()) / desc.CellSpacing;
	float d = (z - 0.5f * Depth()) / -desc.CellSpacing;
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float h00 = heightMap->Data(row, col);
	float h01 = heightMap->Data(row, col + 1);
	float h10 = heightMap->Data(row + 1, col);
	float h11 = heightMap->Data(row + 1, col + 1);

	float s = c - col;
	float t = d - row;

	float uy, vy;
	if (s + t <= 1.0f)
	{
		uy = h01 - h00;
		vy = h01 - h11;

		return h00 + (1.0f - s) * uy + (1.0f - t) * vy;
	}

	uy = h10 - h11;
	vy = h01 - h11;

	return h11 + (1.0f - s) * uy + (1.0f - t) * vy;
}

bool Terrain::Picking(OUT D3DXVECTOR3 * pickPixel, OUT D3DXVECTOR3 * pickWorldPos)
{
	float x, z;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXVECTOR4 result = Texture::ReadPixel128(render->GetRenderTargetTexture(), (UINT)position.x, (UINT)position.y);
	x = result.x;
	z = result.y;

	if (pickPixel != NULL)
	{
		pickPixel->x = x * (desc.HeightMapWidth - 1.0f);
		pickPixel->z = z * (desc.HeightMapHeight - 1.0f);
	}

	if (pickWorldPos != NULL)
	{
		pickWorldPos->x = x * Width() - (Width() * 0.5f);
		pickWorldPos->z = z * Depth() - (Depth() * 0.5f);
	}

	return x + z > 0;
}

bool Terrain::IsPicked()
{
	return isPick;
}

void Terrain::SetPickState(bool val)
{
	isPick = val;
}

GData* Terrain::Save()
{
	TerrainData*data = new TerrainData();

	data->heightMap = desc.HeightMap;
	data->heightScale = desc.HeightScale;
	data->heightMapWidth = desc.HeightMapWidth;
	data->heightMapHeight = desc.HeightMapHeight;

	data->layerMap[0] = desc.layerMapFile[0];
	data->layerMap[1] = desc.layerMapFile[1];
	data->layerMap[2] = desc.layerMapFile[2];

	data->blendHeight = desc.blendHeight;
	data->activeBlend = desc.activeBlend;

	data->DetailValue = detail.DetailValue;
	data->DetailIntensity = detail.DetailIntensity;

	data->ambient = desc.material->GetAmbient();

	if (desc.material->GetDiffuseMap() == NULL)
		data->diffuseMap = L"";
	else
		data->diffuseMap = desc.material->GetDiffuseMap()->GetFile();
	data->diffuse = desc.material->GetDiffuse();

	if (desc.material->GetSpecularMap() == NULL)
		data->specularMap = L"";
	else
		data->specularMap = desc.material->GetSpecularMap()->GetFile();
	data->specular = desc.material->GetSpecular();
	data->intensity = desc.material->GetSpecular().a;

	if (desc.material->GetNormalMap() == NULL)
		data->normalMap = L"";
	else
		data->normalMap = desc.material->GetNormalMap()->GetFile();
	if (desc.material->GetDetailMap() == NULL)
		data->detailMap = L"";
	else
		data->detailMap = desc.material->GetDetailMap()->GetFile();

	data->texScale = GetTerrainRender()->GetBuffer().TexScale;
	data->fogStart = GetTerrainRender()->GetBuffer().FogStart;
	data->fogRange = GetTerrainRender()->GetBuffer().FogRange;


	if (tree != NULL)
	{
		for (size_t i = 0; i < tree->GetVertices().size(); i++)
		{
			TerrainData::BillboardInfo bInfo;

			bInfo.position = tree->GetVertices()[i].TransPosition;

			D3DXMATRIX rotation = tree->GetVertices()[i].RotateWorld;

			D3DXQUATERNION q;
			D3DXQuaternionRotationMatrix(&q, &rotation);

			D3DXVECTOR3 rot;

			Math::toEulerAngle(q, rot);

			rot.x = Math::ToDegree(rot.x);
			rot.y = Math::ToDegree(rot.y);
			rot.z = Math::ToDegree(rot.z);

			bInfo.rotate = rot;
			bInfo.size = tree->GetVertices()[i].Size;
			bInfo.textureNum = tree->GetVertices()[i].TextureNumber;

			data->billboards.push_back(bInfo);
		}

		for (size_t i = 0; i < tree->GetBillboardCollider().size(); i++)
		{
			TerrainData::BillboardCollider cInfo;

			cInfo.position = tree->GetBillboardCollider()[i].position;
			cInfo.scale = tree->GetBillboardCollider()[i].scale;

			data->billboardCollider.push_back(cInfo);
		}
	}

	data->loadFileData = loadFileName;

	return (GData*)data;
}
