#include "stdafx.h"
#include "Trail.h"

#include "../Objects/GameModel.h"

Trail::Trail()
	: deltaTime(0), elapsedTime(0.0001f)
	, bBone(false), bCull(false), bDepth(false), bRender(true)
{
	parentName = L"None";
	worldBuffer = new WorldBuffer();
	vsBuffer = new VsBuffer();
	vsBuffer->Data.Count = 128;

	int count = vsBuffer->Data.Count;

	psBuffer = new PsBuffer();

	texture = new Texture(Textures + L"BlendTest.png");

	// shader
	shader = new Shader(Shaders + L"Trail.hlsl");

	// Create Vertex Buffer
	{
		VertexTexture* vertices = new VertexTexture[6 * count];

		float dx = 1.0f / count; // ∫Ò¿≤

		for (UINT i = 0; i < count; i++)
		{
			//count∞° ƒø¡ˆ∏È √Œ√Œ«œ∞‘ ≥™ø¿∞‘ µ 
			vertices[i * 6 + 0].Position = D3DXVECTOR3(-0.5f + (dx * i), -0.5f, 0.0f);
			vertices[i * 6 + 1].Position = D3DXVECTOR3(-0.5f + (dx * i), 0.5f, 0.0f);
			vertices[i * 6 + 2].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 3].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 4].Position = D3DXVECTOR3(-0.5f + (dx * i), 0.5f, 0.0f);
			vertices[i * 6 + 5].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), 0.5f, 0.0f);

			vertices[i * 6 + 0].Uv = D3DXVECTOR2((dx * i), 1);
			vertices[i * 6 + 1].Uv = D3DXVECTOR2((dx * i), 0);
			vertices[i * 6 + 2].Uv = D3DXVECTOR2(dx * (i + 1), 1);
			vertices[i * 6 + 3].Uv = D3DXVECTOR2(dx * (i + 1), 1);
			vertices[i * 6 + 4].Uv = D3DXVECTOR2((dx * i), 0);
			vertices[i * 6 + 5].Uv = D3DXVECTOR2(dx * (i + 1), 0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6 * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->CullMode(D3D11_CULL_NONE);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->DestBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendMode[1]->BlendOp(D3D11_BLEND_OP_ADD);

	D3DXMatrixIdentity(&worldMatrix);

	model = NULL;

	setPosition = { 0,0,0 };
	setRotation = { 0,0,0 };
	setScale = { 1,1,1 };
}

Trail::~Trail()
{
	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);

	SAFE_DELETE(blendMode[0]);
	SAFE_DELETE(blendMode[1]);

	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(texture);
	SAFE_DELETE(shader);
	SAFE_DELETE(vsBuffer);
	SAFE_DELETE(psBuffer);
	SAFE_DELETE(worldBuffer);
}

void Trail::Update()
{
	deltaTime += Time::Delta();

	if (deltaTime >= elapsedTime)
	{
		deltaTime = 0;
		ShiftMatrix();
	}
}

void Trail::Render()
{
	if (!bRender)return;

	worldBuffer->SetMatrix(World());
	worldBuffer->SetVSBuffer(1);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vsBuffer->SetVSBuffer(7);
	psBuffer->SetPSBuffer(7);
	texture->SetShaderResource(5);

	blendMode[1]->OMSetBlendState();
	if (bDepth == false)
		depthMode[1]->OMSetDepthStencilState();
	if (bCull == false)
		cullMode[1]->RSSetState();
	shader->Render();
	D3D::GetDC()->Draw(6 * vsBuffer->Data.Count, 0);
	cullMode[0]->RSSetState();
	depthMode[0]->OMSetDepthStencilState();
	blendMode[0]->OMSetBlendState();
}

void Trail::ImGuiRender()
{
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ParentObject]");
	ImGui::Text("%s", String::ToString(parentName).c_str());
	ImGui::Separator();

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Trail]");
	if (ImGui::Button("Show Trail"))
	{
		if (!bRender)bRender = true;
		else bRender = false;
	}
	ImGui::DragFloat("ElapsedTime", &elapsedTime, 0.1f, 0, 0, "%.5f");
	ImGui::DragInt("TrailCount", &vsBuffer->Data.Count, 0.5f, 32, 128);

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetSRT]");
	wstring textTrans = L"[" + trailName + L"]translate";
	ImGui::Text(String::ToString(textTrans).c_str());
	if (ImGui::DragFloat3("Position", (float*)&setPosition, 0.1f))
		Position(setPosition);

	wstring textScale = L"[" + trailName + L"]rotate";
	ImGui::Text(String::ToString(textScale).c_str());
	if (ImGui::DragFloat3("Rotation", (float*)&setRotation, 0.02f))
		RotationDegree(setRotation);

	wstring textRotate = L"[" + trailName + L"]scale";
	ImGui::Text(String::ToString(textRotate).c_str());
	if (ImGui::DragFloat3("Scale", (float*)&setScale, 0.1f))
		Scale(setScale);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Mode]");
	ImGui::Checkbox("CullMode", &bCull);
	ImGui::Checkbox("DepthMode", &bDepth);
	ImGui::Separator();
	
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Texture]");
	
	ImGui::Checkbox("UseTexture", (bool*)&psBuffer->Data.UseTexture);
	ImGui::ColorEdit4("Color", (float*)&psBuffer->Data.Color);
	ImGui::Separator();
}


void Trail::ShiftMatrix()
{
	if (bBone) {
		worldMatrix = World() * model->GetBoneMatrix(boneName);
	}
	else {
		worldMatrix = model->World();
		worldMatrix = World() * worldMatrix;
	}
	for (int i = vsBuffer->Data.Count - 1; i > 0; i--) {
		vsBuffer->Data.Worlds[i] = vsBuffer->Data.Worlds[i - 1];
	}

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	vsBuffer->Data.Worlds[0] = worldMatrix;
}
