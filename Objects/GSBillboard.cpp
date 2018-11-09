#include "stdafx.h"
#include "GSBillboard.h"

GSBillboard::GSBillboard(ExecuteValues * values, int billBoardCount, D3DXVECTOR2 scale, D3DXVECTOR2 position)
	:values(values)
{
	shader = new Shader(Shaders + L"048_Billboard.hlsl");
	shader->CreateGS();
	worldBuffer = new WorldBuffer();

	vertexCount = billBoardCount;
	beforeCount = vertexCount;
	setPos = beforePos= position;
	setScale = beforeScale= scale;

	vertices = new VertexSize[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		D3DXVECTOR2 Scale;

		Scale.x = Math::Random(1.0f, setScale.x);
		Scale.y = Math::Random(1.0f, setScale.y);

		D3DXVECTOR3 Position;
		Position.x = Math::Random(-setPos.x, setPos.x);
		Position.y = Scale.y*0.5f;
		Position.z = Math::Random(-setPos.y, setPos.y);
		
		vertices[i].Position = Position;
		vertices[i].Size = Scale;
	}

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexSize) * billBoardCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertices;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FillMode(D3D11_FILL_WIREFRAME);
}

GSBillboard::~GSBillboard()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(texture);
	SAFE_DELETE_ARRAY(vertices);
}

void GSBillboard::Update()
{
	if (vertexCount != beforeCount)
	{
		SAFE_DELETE_ARRAY(vertices);
		vertices = new VertexSize[vertexCount];
		for (int i = 0; i < vertexCount; i++)
		{
			D3DXVECTOR2 Scale;

			Scale.x = Math::Random(1.0f, setScale.x);
			Scale.y = Math::Random(1.0f, setScale.y);

			D3DXVECTOR3 Position;
			Position.x = Math::Random(-setPos.x, setPos.x);
			Position.y = Scale.y*0.5f;
			Position.z = Math::Random(-setPos.y, setPos.y);

			vertices[i].Position = Position;
			vertices[i].Size = Scale;
		}
		D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexSize), vertexCount);
		beforeCount = vertexCount;
	}

	if (setPos != beforePos)
	{
		for (int i = 0; i < vertexCount; i++)
		{
			D3DXVECTOR2 Scale;

			Scale.x = Math::Random(1.0f, setScale.x);
			Scale.y = Math::Random(1.0f, setScale.y);

			D3DXVECTOR3 Position;
			Position.x = Math::Random(-setPos.x, setPos.x);
			Position.y = Scale.y*0.5f;
			Position.z = Math::Random(-setPos.y, setPos.y);

			vertices[i].Position = Position;
			vertices[i].Size = Scale;
		}
		D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexSize), vertexCount);
		beforePos = setPos;
	}

	if (setScale != beforeScale)
	{
		for (int i = 0; i < vertexCount; i++)
		{
			D3DXVECTOR2 Scale;

			Scale.x = Math::Random(1.0f, setScale.x);
			Scale.y = Math::Random(1.0f, setScale.y);

			D3DXVECTOR3 Position;
			Position.x = Math::Random(-setPos.x, setPos.x);
			Position.y = Scale.y*0.5f;
			Position.z = Math::Random(-setPos.y, setPos.y);

			vertices[i].Position = Position;
			vertices[i].Size = Scale;
		}
		D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexSize), vertexCount);
		beforeScale = setScale;
	}
}

void GSBillboard::Render()
{
	UINT stride = sizeof(VertexSize);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	values->ViewProjection->SetGSBuffer(0);

	worldBuffer->SetGSBuffer(1);
	shader->Render();
	texture->SetShaderResource(10);

	D3D::GetDC()->Draw(1000, 0);
	D3D::GetDC()->GSSetShader(NULL, NULL, 0);
}

void GSBillboard::ImGuiRender()
{
	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "=========[%s]=========", String::ToString(name).c_str());
	ImGui::BulletText("BliiboardCount : %.d", vertexCount);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetPosition]");
	ImGui::DragFloat2("Position", (float*)&setPos, 0.1f);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetScale]");
	ImGui::DragFloat2("Scale", (float*)&setScale,0.1f,1);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetBillboardCount]");
	ImGui::DragInt("Count", &vertexCount,0.5f,0,2000);
	ImGui::Separator();


}
