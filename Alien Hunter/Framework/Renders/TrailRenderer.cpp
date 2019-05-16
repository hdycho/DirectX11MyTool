#include "FrameWork.h"
#include "TrailRenderer.h"
#include "Draw\LineMaker.h"


TrailRenderer::TrailRenderer(GameModel * model, Effect * effect)
	: model(model), effect(effect)
	, run(false), count(0), lifeTime(0.0f)
	, vertices(NULL), indices(NULL)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, boneIndex(-2), isLoad(false)
{
	isModel = false;
	lineEffect = new Effect(Effects + L"001_Line.hlsl");

	diffuse = new Texture(Textures + L"Box.png");
	alpha = new Texture(Textures + L"Bricks.png");

	trailMapVar = this->effect->AsSRV("TrailMap");
	alphaMapVar = this->effect->AsSRV("AlphaMap");

	D3DXMatrixIdentity(&startPos);
	D3DXMatrixIdentity(&endPos);

	UINT handIdx;
	if (model->GetModel()->BoneCount() < 2)
	{
		isModel = true;
		handIdx = 0;
		boneMatrix = model->World();
	}
	else
	{
		handIdx = model->GetModel()->BoneByName(L"RightHand")->Index();
		boneMatrix = model->GetBoneMatrix(handIdx);
	}

	lines = NULL;
	debugLine[0] = NULL;
	debugLine[1] = NULL;
}

TrailRenderer::~TrailRenderer()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(diffuse);
	SAFE_DELETE(alpha);

	SAFE_DELETE(lineEffect);
	SAFE_DELETE(debugLine[0]);
	SAFE_DELETE(debugLine[1]);
	SAFE_DELETE(lines);
}

void TrailRenderer::Ready()
{
	if (isLoad)return;
	lines = new LineMaker();
	BBox box = BBox(D3DXVECTOR3(-0.1f, -0.1f, -0.1f), D3DXVECTOR3(0.1f, 0.1f, 0.1f));
	for (int i = 0; i < 2; i++)
	{
		debugLine[i] = new DebugLine(lineEffect);
		debugLine[i]->Ready();
		debugLine[i]->Color(0, 1, 1);
		debugLine[i]->Draw(startPos, &box);
	}
}

void TrailRenderer::Update()
{
	if (run == false) return;

	for (size_t i = 0; i < linePoints[0].size(); i++)
	{
		linePoints[0][i].Time += Time::Delta();

		if (linePoints[0][i].Time >= lifeTime)
		{
			linePoints[0].erase(linePoints[0].begin() + i);
			linePoints[1].erase(linePoints[1].begin() + i);
		}
	}

	if (boneIndex == -2) // 아무것도 안들어간상태
	{
		if (!isModel)
		{
			UINT handIdx = model->GetModel()->BoneByName(L"RightHand")->Index();
			boneMatrix = model->GetPlayerBoneWorld(handIdx);
		}
		else
		{
			boneMatrix = model->GetPlayerBoneWorld(boneIndex);
		}
	}
	else
		boneMatrix = model->GetPlayerBoneWorld(boneIndex);

	D3DXMATRIX startMat = startPos * boneMatrix;
	D3DXMATRIX endMat = endPos * boneMatrix;

	D3DXVECTOR3 pos = D3DXVECTOR3(startMat._41, startMat._42, startMat._43);
	D3DXVECTOR3 pos2 = D3DXVECTOR3(endMat._41, endMat._42, endMat._43);

	MyVertex vec[2];
	vec[0].Position = pos;
	vec[1].Position = pos2;

	count += Time::Delta();

	float invFrame = 1.0f / 30.0f;
	if (count > invFrame)
	{
		count = 0;

		if (linePoints[0].size() < 50)
		{
			linePoints[0].push_back(vec[0]);
			linePoints[1].push_back(vec[1]);
		}

		MakeRomLines();
		CreateTexture();
	}
}

void TrailRenderer::PreRender()
{
}

void TrailRenderer::Render()
{
	if (run == false) return;
	if (start.size() < 2) return;

	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	trailMapVar->SetResource(diffuse->SRV());
	alphaMapVar->SetResource(alpha->SRV());
	effect->DrawIndexed(0, 0, indexCount);
}

void TrailRenderer::ImGuiRender()
{
	D3DXMATRIX startMat = startPos * boneMatrix;
	D3DXMATRIX endMat = endPos * boneMatrix;
	
	debugLine[0]->UpdateWorld(startMat);
	debugLine[0]->Render();
	debugLine[1]->UpdateWorld(endMat);
	debugLine[1]->Render();

	D3DXVECTOR3 resStartPos = { 0,0,0 };
	D3DXVECTOR3 resEndPos = { 0,0,0 };
	D3DXVec3TransformCoord(&resStartPos, &resStartPos, &startMat);
	D3DXVec3TransformCoord(&resEndPos, &resEndPos, &endMat);

	lines->UpdateLine(resStartPos, resEndPos);
	lines->DrawLine(1);

	D3DXVECTOR3 trans;

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Life Time");
	ImGui::DragFloat("##Life Time", &lifeTime, 0.01f, 0, 10.0f);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Target Index");
	ImGui::DragInt("##t idx", &boneIndex, 0.01f, 0,65);
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		trans = D3DXVECTOR3(startPos._41, startPos._42, startPos._43);
		ImGui::Text("StartPos");
		if (ImGui::DragFloat3("##StartPos", (float*)&trans, 0.1f, -200, 200))
			D3DXMatrixTranslation(&startPos, trans.x, trans.y, trans.z);

		trans = D3DXVECTOR3(endPos._41, endPos._42, endPos._43);
		ImGui::Text("EndPos");
		if (ImGui::DragFloat3("##EndPos", (float*)&trans, 0.1f, -200, 200))
			D3DXMatrixTranslation(&endPos, trans.x, trans.y, trans.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Diffuse Texture");
		if (ImGui::ImageButton(diffuse->SRV(), ImVec2(100, 100)))
			Path::OpenFileDialog(L"파일", L".png", Textures, bind(&TrailRenderer::LoadTexture, this, placeholders::_1, 0));
		
		ImGui::Text("Alpha Texture");
		if (ImGui::ImageButton(alpha->SRV(), ImVec2(100, 100)))
			Path::OpenFileDialog(L"파일", L".png", Textures, bind(&TrailRenderer::LoadTexture, this, placeholders::_1, 0));
		ImGui::TreePop();
	}

	PostRender();
}

void TrailRenderer::PostRender()
{
	D3DXMATRIX V, P;
	Context::Get()->GetMainCamera()->Matrix(&V);
	Context::Get()->GetPerspective()->GetMatrix(&P);

	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &V, &P);

	D3DXMATRIX WVP = startPos * boneMatrix*matrix;
	D3DXMATRIX WVP2 = endPos * boneMatrix*matrix;

	D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
	D3DXVECTOR4 worldToScreen2(0, 0, 0, 1);
	D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);
	D3DXVec4Transform(&worldToScreen2, &worldToScreen2, &WVP2);

	//NDC 공간으로 변환
	float wScreenX = worldToScreen.x / worldToScreen.w;
	float wScreenY = worldToScreen.y / worldToScreen.w;
	float wScreenZ = worldToScreen.z / worldToScreen.w;

	//NDC 공간으로 변환
	float wScreenX2 = worldToScreen2.x / worldToScreen2.w;
	float wScreenY2 = worldToScreen2.y / worldToScreen2.w;
	float wScreenZ2 = worldToScreen2.z / worldToScreen2.w;

	//-1~1 구간을 0~1구간으로 변환
	float nScreenX = (wScreenX + 1)*0.5f;
	float nScreenY = 1.0f - (wScreenY + 1)*0.5f;

	//-1~1 구간을 0~1구간으로 변환
	float nScreenX2 = (wScreenX2 + 1)*0.5f;
	float nScreenY2 = 1.0f - (wScreenY2 + 1)*0.5f;

	//최종화면의 좌표
	float resultX = nScreenX*Context::Get()->GetViewport()->GetWidth();
	float resultY = nScreenY*Context::Get()->GetViewport()->GetHeight();

	//최종화면의 좌표
	float resultX2 = nScreenX2*Context::Get()->GetViewport()->GetWidth();
	float resultY2 = nScreenY2*Context::Get()->GetViewport()->GetHeight();

	RECT rect = { resultX,resultY,resultX + 100,resultY + 100 };
	RECT rect2= { resultX2,resultY2,resultX2 + 100,resultY2 + 100 };

	string str;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("BCKGND", NULL, ImGui::GetIO().DisplaySize, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		str = "Start";
		ImGui::GetWindowDrawList()->AddText(ImVec2((float)rect.left, (float)rect.top), ImColor(1.0f, 1.0f, 1.0f, 1.0f), str.c_str());

		str = "End";
		ImGui::GetWindowDrawList()->AddText(ImVec2((float)rect2.left, (float)rect2.top), ImColor(1.0f, 1.0f, 1.0f, 1.0f), str.c_str());
	}
	ImGui::End();
}

void TrailRenderer::ResizeScreen()
{
}

void TrailRenderer::ResetTrail()
{
	for (int i = 0; i < 2; i++)
		linePoints[i].clear();

	start.clear();
	end.clear();
}


void TrailRenderer::MakeRomLines()
{
	///////////////////////////////////////////////////////////////////////////////
	//	D3DXVec3CatmullRom
	//	(
	//		D3DXVECTOR3* pOut,
	//		CONST D3DXVECTOR3* pV1,
	//		CONST D3DXVECTOR3* pV2,
	//		CONST D3DXVECTOR3* pV3,
	//		CONST D3DXVECTOR3* pV4,
	//		FLOAT s
	//	)
	//	위 함수는 각각 4개의 점과 0~1사이의 하나의 가중치 값(s)을 받으며
	//	pOut에 보간 된 벡터를 반환한다
	//	만약 s 가 0이면 pv2를 리턴하게 되며 1일 경우 pv3를 리턴하게 된다
	///////////////////////////////////////////////////////////////////////////////

	start.clear();
	end.clear();

	for (int i = 0; i < 2; i++)
	{
		int catmullNum = linePoints[i].size() - 3;
		if (catmullNum < 1) break;

		int size = linePoints[i].size();

		D3DXVECTOR3 v;

		for (int k = -1; k < catmullNum; k++)
		{
			for (int j = 0; j < 10; j++)
			{
				float factor = (float)j / 10.0f;

				if (k == -1)
				{
					D3DXVec3CatmullRom(&v,
						&linePoints[i][size - 1].Position,
						&linePoints[i][size - 1].Position,
						&linePoints[i][size - 2].Position,
						&linePoints[i][size - 3].Position,
						factor);

					if (i == 0)
						start.push_back(v);
					else
						end.push_back(v);
				}
				else
				{
					D3DXVec3CatmullRom(&v,
						&linePoints[i][size - (1 + k)].Position,
						&linePoints[i][size - (2 + k)].Position,
						&linePoints[i][size - (3 + k)].Position,
						&linePoints[i][size - (4 + k)].Position,
						factor);

					if (i == 0)
						start.push_back(v);
					else
						end.push_back(v);
				}
			}
		}
	}

	for (size_t i = 0; i < start.size(); i++)
	{
		D3DXVECTOR3 temp = end[i] - start[i];

		end[i] -= temp * ((float)(i) / (float)start.size());
	}
}

void TrailRenderer::CreateTexture()
{
	if (start.size() < 2) return;

	UINT width = start.size() - 1;
	UINT height = 1;

	float count = 0;
	//	Set Vertices
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexTextureNormal[vertexCount];

		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				D3DXVECTOR3 position;
				if (z == 0)
					position = start[x];
				else
					position = end[x];

				vertices[index].Position.x = position.x;
				vertices[index].Position.y = position.y;
				vertices[index].Position.z = position.z;

				vertices[index].Uv.x = (float)x / (float)width;
				vertices[index].Uv.y = (float)z / (float)height;
			}
		}
	}

	//	Set Indices
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * z + x;
				indices[index + 1] = (width + 1) * (z + 1) + x;
				indices[index + 2] = (width + 1) * z + (x + 1);

				indices[index + 3] = (width + 1) * z + (x + 1);
				indices[index + 4] = (width + 1) * (z + 1) + x;
				indices[index + 5] = (width + 1) * (z + 1) + (x + 1);

				index += 6;
			}
		}
	}

	//	Set Normal
	{
		for (UINT i = 0; i < (indexCount / 3); i++)
		{
			UINT index0 = indices[i * 3 + 0];
			UINT index1 = indices[i * 3 + 1];
			UINT index2 = indices[i * 3 + 2];

			D3DXVECTOR3 v0 = vertices[index0].Position;
			D3DXVECTOR3 v1 = vertices[index1].Position;
			D3DXVECTOR3 v2 = vertices[index2].Position;

			D3DXVECTOR3 dV1 = v1 - v0;
			D3DXVECTOR3 dV2 = v2 - v0;

			D3DXVECTOR3 normal;
			D3DXVec3Cross(&normal, &dV1, &dV2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}

		for (UINT i = 0; i < vertexCount; i++)
			D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}

	CreateBuffer();

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void TrailRenderer::CreateBuffer()
{
	//	Create Vertex Buffer
	{
		SAFE_RELEASE(vertexBuffer);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//	Create Index Buffer
	{
		SAFE_RELEASE(indexBuffer);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void TrailRenderer::LoadTexture(wstring file, UINT type)
{
	switch (type)
	{
		case 0:
			if (diffuse != NULL)
				SAFE_DELETE(diffuse);

			diffuse = new Texture(file);
			trailMapVar->SetResource(diffuse->SRV());

			break;
		case 1:
			if (alpha != NULL)
				SAFE_DELETE(alpha);

			alpha = new Texture(file);
			alphaMapVar->SetResource(alpha->SRV());
			break;
	}
}
