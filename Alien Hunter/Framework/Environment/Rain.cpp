#include "Framework.h"
#include "Rain.h"
#include "../Viewer/Freedom.h"
#include "Datas\GameData.h"

Rain::Rain(D3DXVECTOR3 & size, UINT count)
	: particleCount(count), size(size), isPicked(false)
	, name(L""), velocity(D3DXVECTOR3(0, -200, 0)), drawDist(1000.0f)
	, color(1, 1, 1, 1)
{
}

Rain::Rain(Rain & copy)
	: size(size), isPicked(false)
	, name(L""), velocity(D3DXVECTOR3(0, -200, 0)), drawDist(1000.0f)
	, color(1, 1, 1, 1)
{
	size = copy.size;
	color = copy.color;
	drawDist = copy.drawDist;
	velocity = copy.velocity;
	particleCount = copy.particleCount;
	loadFileName = copy.loadFileName;
}

Rain::~Rain()
{
	SAFE_DELETE(rainMaps);

	SAFE_DELETE(effect);

	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(vertices);
}

void Rain::Ready()
{
	effect = new Effect(Effects + L"005_Rain.hlsl");

	vertices = new VertexRain[particleCount];
	//	Create Data
	{
		for (UINT i = 0; i < particleCount; i++)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.1f, 0.4f);
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

			UINT textureID = (UINT)Math::Random(0, 370);

			//this number is used to randomly increase the brightness of some rain particles
			float random = 1;
			float randomIncrease = float((double)rand() / ((double)(RAND_MAX)+(double)(1)));
			if (randomIncrease > 0.8)
				random += randomIncrease;

			vertices[i] = VertexRain(P, S, textureID, random);
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexRain) * particleCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	vector<wstring> textureArray;
	for (int i = 0; i < 370; i++)
	{
		wstring temp;
		if (i >= 100)
			temp = Textures + L"rainTextures/cv0_vPositive_0";
		else if (i >= 10)
			temp = Textures + L"rainTextures/cv0_vPositive_00";
		else
			temp = Textures + L"rainTextures/cv0_vPositive_000";

		textureArray.push_back(temp.append(to_wstring(i) + L".dds"));
	}

	rainMaps = new TextureArray(textureArray);

	srvVariable= effect->AsSRV("Map");
	sizeVariable = effect->AsVector("Size");
	velocityVariable = effect->AsVector("Velocity");
	distVariable = effect->AsScalar("DrawDistance");
	colorVariable = effect->AsVector("Color");
	originVariable = effect->AsVector("Origin");
	timeVariable = effect->AsScalar("RainTime");

	srvVariable->SetResource(rainMaps->GetSRV());
	sizeVariable->SetFloatVector(size);
	velocityVariable->SetFloatVector(velocity);
	distVariable->SetFloat(drawDist);
	colorVariable->SetFloatVector(color);
	originVariable->SetFloatVector(D3DXVECTOR3(0, 0, 0));
}

void Rain::Update()
{
	float rainTime = Time::Get()->Running() * 1000.0f / 750.0f;
	timeVariable->SetFloat(rainTime);

	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	originVariable->SetFloatVector(camPos);
}

void Rain::Render()
{
	UINT stride = sizeof(VertexRain);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	effect->Draw(0, 0, particleCount);
}

void Rain::ImGuiRender()
{
	D3DXVECTOR3 preSize = size;
	UINT preCount = particleCount;

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "===============[%s]===============", String::ToString(name).c_str());
	ImGui::Separator();
	ImGui::Separator();

	ImGui::Text("Size");
	ImGui::SliderFloat3("##s", size, 0.0f, 1000, "%.5f");
	sizeVariable->SetFloatVector(size);

	if (size.x != preSize.x || size.y != preSize.y || size.z != preSize.z)
		UpdateVertexResource();
	ImGui::Separator();

	ImGui::Text("RainCount");
	ImGui::DragInt("##rc", (int*)&particleCount, 1, 0, 10000);
	if (preCount != particleCount)
		UpdateVertexResource();
	ImGui::Separator();

	ImGui::Text("Velocity");
	ImGui::SliderFloat3("##velo", velocity, -1000, 1000, "%.5f");
	velocityVariable->SetFloatVector(velocity);
	ImGui::Separator();

	ImGui::Text("DrawDistance");
	ImGui::DragFloat("##dist", &drawDist, 0.5f, 0, 1000);
	distVariable->SetFloat(drawDist);
	ImGui::Separator();

	ImGui::Text("Color");
	ImGui::ColorEdit4("Time Factor", color);
	colorVariable->SetFloatVector(color);
}

void Rain::UpdateVertexResource()
{
	SAFE_DELETE_ARRAY(vertices);
	vertices = new VertexRain[particleCount];
	//	Create Data
	{
		for (UINT i = 0; i < particleCount; i++)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.1f, 0.4f);
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

			UINT textureID = (UINT)Math::Random(0, 370);

			//this number is used to randomly increase the brightness of some rain particles
			float random = 1;
			float randomIncrease = float((double)rand() / ((double)(RAND_MAX)+(double)(1)));
			if (randomIncrease > 0.8)
				random += randomIncrease;

			vertices[i] = VertexRain(P, S, textureID, random);
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexRain), particleCount);
}

GData* Rain::Save()
{
	RainData*data = new RainData();

	data->size = size;
	data->color = color;
	data->dist = drawDist;
	data->velocity = velocity;
	data->particleCount = particleCount;
	data->loadFileData = loadFileName;

	return (GData*)data;
}

wstring & Rain::Name()
{
	return name;
}

void Rain::Load(wstring fileName)
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	loadFileName = fileName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement();
	wstring name = String::ToWString(node->GetText());

	node = node->NextSiblingElement(); // RainSize
	Xml::XMLElement*rainSize = node->FirstChildElement();
	size.x = rainSize->FloatText();

	rainSize = rainSize->NextSiblingElement();
	size.y = rainSize->FloatText();

	rainSize = rainSize->NextSiblingElement();
	size.z = rainSize->FloatText();

	node = node->NextSiblingElement(); // Velocity
	Xml::XMLElement*rainVel = node->FirstChildElement();
	velocity.x = rainVel->FloatText();

	rainVel = rainVel->NextSiblingElement();
	velocity.y = rainVel->FloatText();

	rainVel = rainVel->NextSiblingElement();
	velocity.z = rainVel->FloatText();

	node = node->NextSiblingElement(); // DrawDist
	drawDist = node->FloatText();

	node = node->NextSiblingElement(); // RainColor
	Xml::XMLElement*rainCol = node->FirstChildElement();
	color.r = rainCol->FloatText();

	rainCol = rainCol->NextSiblingElement();
	color.g = rainCol->FloatText();

	rainCol = rainCol->NextSiblingElement();
	color.b = rainCol->FloatText();

	rainCol = rainCol->NextSiblingElement();
	color.a = rainCol->FloatText();

	node = node->NextSiblingElement(); // DrawDist
	particleCount = node->IntText();

	SAFE_DELETE(document);
}

bool Rain::IsPicked()
{
	return isPicked;
}

void Rain::SetPickState(bool val)
{
	isPicked = val;
}
