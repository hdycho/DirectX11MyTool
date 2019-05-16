#include "Framework.h"
#include "Billboard.h"
#include "Environment\Terrain.h"
#include "Environment\TerrainRender.h"
#include "Viewer\Frustum.h"
#include "Collider\ColliderBox.h"

Billboard::Billboard(class Terrain*terrain, Effect*lineEffect)
	:billboardTexture(NULL)
	, terrain(terrain)
	, BillboardCount(0)
	, lineEffect(lineEffect)
{
	vertexBuffer[0] = NULL;
	vertexBuffer[1] = NULL;
	bType = BillboardType::TWO;
	vertices.clear();
}

Billboard::Billboard(Billboard & copy)
	:billboardTexture(NULL)
	, terrain(terrain)
	, BillboardCount(0)
{
	vertexBuffer[0] = NULL;
	vertexBuffer[1] = NULL;
	bType = BillboardType::TWO;
	vertices.clear();

	vertices.assign(copy.vertices.begin(), copy.vertices.end());
	billboardCollider.assign(copy.billboardCollider.begin(), copy.billboardCollider.end());
	terrain = copy.terrain;

}

Billboard::~Billboard()
{
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);
	SAFE_DELETE(BillboardMaterial);
	for (Texture*texture : textures)
		SAFE_DELETE(texture);

	SAFE_DELETE(billboardTexture);

	for (UINT i = 0; i < billboardCollider.size(); i++)
		SAFE_DELETE(billboardCollider[i].Box);

	SAFE_DELETE(frustum);
}

void Billboard::Initalize()
{
	frustum = new Frustum(300);
	vertex.Position = { 0,0,0 };
}

void Billboard::Ready()
{
	BillboardMaterial = new Material(Effects + L"003_Billboard.hlsl");

	textureNames.push_back(Textures + L"Trees/Tree.png");
	textureNames.push_back(Textures + L"Trees/Tree2.png");
	textureNames.push_back(Textures + L"flower2.png");

	textures.push_back(new Texture(Textures + L"Trees/Tree.png"));
	textures.push_back(new Texture(Textures + L"Trees/Tree2.png"));
	textures.push_back(new Texture(Textures + L"flower2.png"));

	billboardTexture = new TextureArray(textureNames);
	
	srvVariable = BillboardMaterial->GetEffect()->AsSRV("Map");
	fogStartVariable = BillboardMaterial->GetEffect()->AsScalar("FogStart");
	fogRangeVariable = BillboardMaterial->GetEffect()->AsScalar("FogRange");
	frustumVariable = BillboardMaterial->GetEffect()->AsVector("WorldFrustumPlanes");

	srvVariable->SetResource(billboardTexture->GetSRV());
	fogStartVariable->SetFloat(terrain->GetTerrainRender()->GetBuffer().FogStart);
	fogRangeVariable->SetFloat(terrain->GetTerrainRender()->GetBuffer().FogRange);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	if (vertices.size() > 0)
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(InstanceBillboard) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	for (size_t i = 0; i < billboardCollider.size(); i++)
	{
		D3DXMATRIX colliderS, colliderT;

		D3DXMatrixTranslation(&colliderT, billboardCollider[i].position.x, billboardCollider[i].position.y, billboardCollider[i].position.z);
		D3DXMatrixScaling(&colliderS, billboardCollider[i].scale.x, billboardCollider[i].scale.y, billboardCollider[i].scale.x);

		D3DXMATRIX world = colliderS*colliderT;

		billboardCollider[i].Box->Transform(world);
	}
}

void Billboard::Render()
{
	for (UINT i = 0; i < billboardCollider.size(); i++)
	{
		billboardCollider[i].Box->DrawCollider();
	}

	UINT stride[2] = { sizeof(Vertex),sizeof(InstanceBillboard) };
	UINT offset[2] = { 0,0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	BillboardMaterial->GetEffect()->DrawInstanced(0, 0, 1, vertices.size());
}

void Billboard::Update()
{
	frustum->GetPlanes(plane);
	frustumVariable->SetFloatVectorArray((float*)plane, 0, 6);
}

void Billboard::ImGuiRender()
{
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Create Type]");
	ImGui::RadioButton("Create One", (int*)&bType, 0);
	ImGui::RadioButton("Create Two", (int*)&bType, 1);
	ImGui::RadioButton("Create Four", (int*)&bType, 3);

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Material]");


	ImGui::Text("Ambient Color : (%.2f,%.2f,%.2f)",
		BillboardMaterial->GetAmbient().r,
		BillboardMaterial->GetAmbient().g,
		BillboardMaterial->GetAmbient().b);
	D3DXCOLOR ambient = BillboardMaterial->GetAmbient();
	ImGui::ColorEdit3("##ambient", ambient);
	BillboardMaterial->SetAmbient(ambient);
	ImGui::Separator();
	ImGui::Text("Diffuse Color : (%.2f,%.2f,%.2f)",
		BillboardMaterial->GetDiffuse().r,
		BillboardMaterial->GetDiffuse().g,
		BillboardMaterial->GetDiffuse().b);
	D3DXCOLOR diffuse = BillboardMaterial->GetDiffuse();
	ImGui::ColorEdit3("##diffuse", diffuse);
	BillboardMaterial->SetDiffuse(diffuse);
	ImGui::Separator();
	ImGui::Text("Specular Color : (%.2f,%.2f,%.2f)",
		BillboardMaterial->GetSpecular().r,
		BillboardMaterial->GetSpecular().g,
		BillboardMaterial->GetSpecular().b);
	D3DXCOLOR specular = BillboardMaterial->GetSpecular();
	ImGui::ColorEdit3("##specular", specular);
	BillboardMaterial->SetSpecular(specular);
	ImGui::DragFloat("Shininess", &specular.a, 0.05f, 0, 100);
	BillboardMaterial->SetSpecular(specular);

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Collider]");

	static bool isShowCollider = true;

	ImGui::Checkbox("Show Collider", &isShowCollider);
	if (!isShowCollider)
	{
		for (UINT i = 0; i < billboardCollider.size(); i++)
			billboardCollider[i].Box->ShowLine() = false;
	}
	else
	{
		for (UINT i = 0; i < billboardCollider.size(); i++)
			billboardCollider[i].Box->ShowLine() = true;
	}


	ImGui::Separator();
	ImGui::Separator();

	D3DXVECTOR2 preSize = setSize;
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Setting Value]");
	ImGui::SliderFloat2("Scale", setSize, 1, 50);
	ImGui::BulletText("Billboard Count : %d", vertices.size());
	if (ImGui::Button("Delete", ImVec2(160, 30)))
	{
		RemoveBillboard();
	}

	if (preSize != setSize)
		ResourceUpdate(setSize);

	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Select Image]");
	ImGui::Columns(3);
	ImGui::Text("Tree1"); ImGui::NextColumn();
	ImGui::Text("Tree2"); ImGui::NextColumn();
	ImGui::Text("Tree3"); ImGui::NextColumn();

	ImGui::Image(textures[0]->SRV(), ImVec2(80, 100));
	ImGui::RadioButton("##1", (int*)&treeType, 0);
	ImGui::NextColumn();

	ImGui::Image(textures[1]->SRV(), ImVec2(80, 100));
	ImGui::RadioButton("##2", (int*)&treeType, 1);
	ImGui::NextColumn();

	ImGui::Image(textures[2]->SRV(), ImVec2(80, 100));
	ImGui::RadioButton("##3", (int*)&treeType, 2);
	ImGui::NextColumn();
	ImGui::Columns(1);
	setTextureNumber = treeType;
	//사이즈 설정하면 가장마지막애가 변함 그래야 보이니까
}

void Billboard::BuildBillboard(D3DXVECTOR3 pos)
{
	float theta;
	static int colliderNum = 0;
	switch (bType)
	{
		case Billboard::BillboardType::ONE:
		{
			D3DXVECTOR3 position = pos;
			position.y = terrain->GetHeight(pos.x, pos.z);
			position.y += this->setSize.y / 2.0f;

			D3DXMATRIX rotate;
			D3DXMatrixIdentity(&rotate);

			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, position.x, position.y, position.z);

			D3DXMATRIX DT;
			D3DXMatrixIdentity(&DT);

			InstanceBillboard billboard;

			billboard.TransPosition = position;
			billboard.DeTransWorld = DT;
			billboard.RotateWorld = rotate;
			billboard.Size = setSize;
			billboard.TextureNumber = setTextureNumber;

			vertices.push_back(billboard);

			BCollider bCollider;
			bCollider.Box = new ColliderBox
			(
				L"tree" + to_wstring(colliderNum),
				D3DXVECTOR3(-0.04f, -0.5f, -0.04f),
				D3DXVECTOR3(0.04f, 0.5f, 0.04f),
				lineEffect
			);

			bCollider.Box->Ready();
			bCollider.position = position;
			bCollider.scale = setSize;

			billboardCollider.push_back(bCollider);
		}
		break;
		case Billboard::BillboardType::TWO:
		{
			D3DXVECTOR3 position;
			for (int i = 0; i < 2; i++)
			{
				if (i == 0)
					theta = 35;
				else if (i == 1)
					theta = -35;

				position = pos;
				position.y = terrain->GetHeight(pos.x, pos.z);
				position.y += this->setSize.y / 2.0f;

				D3DXMATRIX rotate;
				D3DXMatrixRotationYawPitchRoll(&rotate, Math::ToRadian(theta), 0, 0);

				D3DXMATRIX T;
				D3DXMatrixTranslation(&T, position.x, position.y, position.z);

				D3DXVECTOR3 xzPosition = position;
				xzPosition.y = 0;

				float radius = Math::Distance(D3DXVECTOR3(0, 0, 0), xzPosition);
				D3DXVECTOR3 resultPos;
				D3DXVec3TransformCoord(&resultPos, &xzPosition, &rotate);

				D3DXVECTOR3 targetVector = xzPosition - resultPos;
				D3DXVec3Normalize(&targetVector, &targetVector);

				float zValue = fabs(radius*sinf(Math::ToRadian(theta)));
				float xValue = fabs(radius - fabs(radius*cosf(Math::ToRadian(-theta))));

				float targetValue = sqrtf(zValue*zValue + xValue*xValue);

				D3DXVECTOR3 setPos =
				{
					targetVector.x*targetValue,
					0,
					targetVector.z*targetValue
				};

				D3DXMATRIX DT;
				D3DXMatrixTranslation(&DT, setPos.x, setPos.y, setPos.z);

				InstanceBillboard billboard;

				billboard.TransPosition = position;
				billboard.DeTransWorld = DT;
				billboard.RotateWorld = rotate;
				billboard.Size = setSize;
				billboard.TextureNumber = setTextureNumber;

				vertices.push_back(billboard);
			}

			BCollider bCollider;
			bCollider.Box = new ColliderBox
			(
				L"tree" + to_wstring(colliderNum),
				D3DXVECTOR3(-0.04f, -0.5f, -0.04f),
				D3DXVECTOR3(0.04f, 0.5f, 0.04f),
				lineEffect
			);

			bCollider.Box->Ready();
			bCollider.position = position;
			bCollider.scale = setSize;

			billboardCollider.push_back(bCollider);
		}
		break;
		case Billboard::BillboardType::FOUR:
		{
			D3DXVECTOR3 position;
			for (int i = 0; i < 4; i++)
			{
				if (i == 0)
					theta = 25;
				else if (i == 1)
					theta = -25;
				else if (i == 2)
					theta = 60;
				else if (i == 3)
					theta = -60;

				position = pos;
				position.y = terrain->GetHeight(pos.x, pos.z);
				position.y += this->setSize.y / 2.0f;

				D3DXMATRIX rotate;
				D3DXMatrixRotationYawPitchRoll(&rotate, Math::ToRadian(theta), 0, 0);

				D3DXMATRIX T;
				D3DXMatrixTranslation(&T, position.x, position.y, position.z);

				D3DXVECTOR3 xzPosition = position;
				xzPosition.y = 0;

				float radius = Math::Distance(D3DXVECTOR3(0, 0, 0), xzPosition);
				D3DXVECTOR3 resultPos;
				D3DXVec3TransformCoord(&resultPos, &xzPosition, &rotate);

				D3DXVECTOR3 targetVector = xzPosition - resultPos;
				D3DXVec3Normalize(&targetVector, &targetVector);

				float zValue = fabs(radius*sinf(Math::ToRadian(theta)));
				float xValue = fabs(radius - fabs(radius*cosf(Math::ToRadian(-theta))));

				float targetValue = sqrtf(zValue*zValue + xValue*xValue);

				D3DXVECTOR3 setPos =
				{
					targetVector.x*targetValue,
					0,
					targetVector.z*targetValue
				};

				D3DXMATRIX DT;
				D3DXMatrixTranslation(&DT, setPos.x, setPos.y, setPos.z);

				InstanceBillboard billboard;

				billboard.TransPosition = position;
				billboard.DeTransWorld = DT;
				billboard.RotateWorld = rotate;
				billboard.Size = setSize;
				billboard.TextureNumber = setTextureNumber;

				vertices.push_back(billboard);
			}

			BCollider bCollider;
			bCollider.Box = new ColliderBox
			(
				L"tree" + to_wstring(colliderNum),
				D3DXVECTOR3(-0.04f, -0.5f, -0.04f),
				D3DXVECTOR3(0.04f, 0.5f, 0.04f),
				lineEffect
			);

			bCollider.Box->Ready();
			bCollider.position = position;
			bCollider.scale = setSize;

			billboardCollider.push_back(bCollider);
		}
		break;
	}

	for (size_t i = 0; i < billboardCollider.size(); i++)
	{
		D3DXMATRIX colliderS, colliderT;

		D3DXMatrixTranslation(&colliderT, billboardCollider[i].position.x, billboardCollider[i].position.y, billboardCollider[i].position.z);
		D3DXMatrixScaling(&colliderS, billboardCollider[i].scale.x, billboardCollider[i].scale.y, billboardCollider[i].scale.x);

		D3DXMATRIX world = colliderS*colliderT;

		billboardCollider[i].Box->Transform(world);
	}


	SAFE_RELEASE(vertexBuffer[1]);
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(InstanceBillboard) * vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &vertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
	assert(SUCCEEDED(hr));
}

void Billboard::RemoveBillboard()
{
	switch (bType)
	{
		case Billboard::BillboardType::ONE:
			if (vertices.size() < 1)
				break;
			vertices.pop_back();
			break;
		case Billboard::BillboardType::TWO:
			if (vertices.size() < 1)
				break;
			vertices.pop_back();
			vertices.pop_back();
			break;
		case Billboard::BillboardType::FOUR:
			if (vertices.size() < 1)
				break;
			vertices.pop_back();
			vertices.pop_back();
			vertices.pop_back();
			vertices.pop_back();
			break;
	}

	if (billboardCollider.size() > 0)
	{
		SAFE_DELETE(billboardCollider[billboardCollider.size() - 1].Box);
		billboardCollider.pop_back();
	}

	SAFE_RELEASE(vertexBuffer[1]);

	if (vertices.size() > 0)
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(InstanceBillboard) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}
}

void Billboard::ResourceUpdate(D3DXVECTOR2 setScale)
{
	if (vertices.size() < 1)
		return;

	for (int i = 0; i < (int)bType + 1; i++)
	{
		vertices[vertices.size() - 1 - i].Size = setScale;
		billboardCollider[billboardCollider.size() - 1].scale = setScale;

		D3DXVECTOR3 position = vertices[vertices.size() - 1].TransPosition;

		position.y = terrain->GetHeight(position.x, position.z);
		position.y += setScale.y / 2.0f;

		billboardCollider[billboardCollider.size() - 1].position = position;

		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		vertices[vertices.size() - 1 - i].TransPosition = position;
		vertices[vertices.size() - 1 - i].Size = setScale;
	}

	SAFE_RELEASE(vertexBuffer[1]);
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(InstanceBillboard) * vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &vertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
	assert(SUCCEEDED(hr));
}

void Billboard::Load(wstring fileName)
{
	//빌보드정보
	{
		Xml::XMLDocument*document = new Xml::XMLDocument();

		wstring tempFile = fileName;
		Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
		assert(error == Xml::XML_SUCCESS);

		Xml::XMLElement*root = document->FirstChildElement();
		Xml::XMLElement*matNode = root->FirstChildElement();

		Xml::XMLElement*node = NULL;

		node = matNode->FirstChildElement(); // Name

		while (true)
		{
			wstring name = String::ToWString(node->GetText());
			wstring point = L"StartBillboard";
			if (name != point)
				node = node->NextSiblingElement();
			else
				break;
		}

		node = node->NextSiblingElement(); // vectorsize
		vertices.resize((size_t)node->FloatText());

		node = node->NextSiblingElement(); // Billboards

		Xml::XMLElement*billboards = NULL;
		billboards = node->FirstChildElement();

		for (size_t i = 0; i < vertices.size(); i++)
		{
			D3DXVECTOR3 pos;
			Xml::XMLElement*bill = billboards->FirstChildElement();
			pos.x = bill->FloatText();

			bill = bill->NextSiblingElement();
			pos.y = bill->FloatText();

			bill = bill->NextSiblingElement();
			pos.z = bill->FloatText();

			D3DXVECTOR3 rotate;
			bill = bill->NextSiblingElement();
			rotate.x = bill->FloatText();

			bill = bill->NextSiblingElement();
			rotate.y = bill->FloatText();

			bill = bill->NextSiblingElement();
			rotate.z = bill->FloatText();

			D3DXVECTOR2 scale;
			bill = bill->NextSiblingElement();
			scale.x = bill->FloatText();

			bill = bill->NextSiblingElement();
			scale.y = bill->FloatText();

			UINT textNum;
			bill = bill->NextSiblingElement();
			textNum = bill->IntText();

			D3DXMATRIX trans;
			D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);

			D3DXMATRIX rotation;
			D3DXMatrixRotationYawPitchRoll(&rotation, Math::ToRadian(rotate.y), 0, 0);

			D3DXVECTOR3 xzPosition = pos;
			xzPosition.y = 0;

			float radius = Math::Distance(D3DXVECTOR3(0, 0, 0), xzPosition);
			D3DXVECTOR3 resultPos;
			D3DXVec3TransformCoord(&resultPos, &xzPosition, &rotation);

			D3DXVECTOR3 targetVector = xzPosition - resultPos;
			D3DXVec3Normalize(&targetVector, &targetVector);

			float zValue = fabs(radius*sinf(Math::ToRadian(rotate.y)));
			float xValue = fabs(radius - fabs(radius*cosf(Math::ToRadian(-rotate.y))));

			float targetValue = sqrtf(zValue*zValue + xValue*xValue);

			D3DXVECTOR3 setPos =
			{
				targetVector.x*targetValue,
				0,
				targetVector.z*targetValue
			};

			D3DXMATRIX DT;
			D3DXMatrixTranslation(&DT, setPos.x, setPos.y, setPos.z);

			vertices[i].TransPosition = pos;
			vertices[i].RotateWorld = rotation;
			vertices[i].DeTransWorld = DT;
			vertices[i].Size = scale;
			vertices[i].TextureNumber = textNum;

			billboards = billboards->NextSiblingElement();
		}
		SAFE_DELETE(document);
	}


	//빌보드콜라이더
	{
		Xml::XMLDocument*document = new Xml::XMLDocument();

		wstring tempFile = fileName;
		Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
		assert(error == Xml::XML_SUCCESS);

		Xml::XMLElement*root = document->FirstChildElement();
		Xml::XMLElement*matNode = root->FirstChildElement();

		Xml::XMLElement*node = NULL;

		node = matNode->FirstChildElement(); // Name

		while (true)
		{
			wstring name = String::ToWString(node->GetText());
			wstring point = L"BillboardCollider";
			if (name != point)
				node = node->NextSiblingElement();
			else
				break;
		}

		node = node->NextSiblingElement(); // BillboardVectorSize
		billboardCollider.resize(node->IntText());

		node = node->NextSiblingElement(); // BillboardCollider

		Xml::XMLElement*billboards = NULL;
		billboards = node->FirstChildElement();

		for (size_t i = 0; i < billboardCollider.size(); i++)
		{
			D3DXVECTOR3 pos;
			Xml::XMLElement*bill = billboards->FirstChildElement();
			pos.x = bill->FloatText();

			bill = bill->NextSiblingElement();
			pos.y = bill->FloatText();

			bill = bill->NextSiblingElement();
			pos.z = bill->FloatText();

			D3DXVECTOR2 scale;
			bill = bill->NextSiblingElement();
			scale.x = bill->FloatText();

			bill = bill->NextSiblingElement();
			scale.y = bill->FloatText();

			billboardCollider[i].position = pos;
			billboardCollider[i].scale = scale;
			billboardCollider[i].Box = new ColliderBox
			(
				L"treeCollider" + to_wstring(i),
				D3DXVECTOR3(-0.04f, -0.5f, -0.04f),
				D3DXVECTOR3(0.04f, 0.5f, 0.04f),
				lineEffect
			);
			billboardCollider[i].Box->Ready();

			billboards = billboards->NextSiblingElement();
		}
		SAFE_DELETE(document);
	}
}

void Billboard::SetShader(D3DXMATRIX v, D3DXMATRIX p)
{
	BillboardMaterial->LightView()->SetMatrix(v);
	BillboardMaterial->LightProjection()->SetMatrix(p);
}

void Billboard::SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv)
{
	BillboardMaterial->ShadowTransform()->SetMatrix(shadowTransform);
	BillboardMaterial->ShadowMap()->SetResource(srv);
}

void Billboard::ShadowUpdate()
{

}

void Billboard::NormalRender()
{
	Render();
}

void Billboard::ShadowRender(UINT tech, UINT pass)
{
	UINT stride[2] = { sizeof(Vertex),sizeof(InstanceBillboard) };
	UINT offset[2] = { 0,0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	BillboardMaterial->GetEffect()->DrawInstanced(1, 1, 1, vertices.size());
}

void Billboard::ColliderUpdate()
{
}





