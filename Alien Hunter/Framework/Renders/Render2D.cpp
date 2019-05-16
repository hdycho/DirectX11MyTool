#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D(wstring shaderFile)
	: position(0, 0), scale(1, 1)
{
	String::Replace(&shaderFile, Shaders, L"");

	wstring file = L"";
	if (shaderFile.length() > 1)
		file = Effects + shaderFile;
	else
		file = Effects + L"011_Render2D.hlsl";

	effect = new Effect(file, true);

	// Create Ortho Matrix
	// 쉐이더에서 직접 만들어 낼 수 있는데(나중에 함) 이번엔 써볼꺼
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		// 깊이 끄고 써서 0으로 해도 되긴함 0,0 하면 충돌 발생할 수 잇어 -1,1 or 0,1로 하면됨
		D3DXMatrixOrthoOffCenterLH(&orthoGraphic, 0, desc.Width, 0, desc.Height, -1, 1);
	}

	// Create Vertex Buffer
	{
		VertexTexture* vertices = new VertexTexture[6];
		// 012 213
		vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
		vertices[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[4].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[5].Position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);

		vertices[0].Uv = D3DXVECTOR2(0, 1);
		vertices[1].Uv = D3DXVECTOR2(0, 0);
		vertices[2].Uv = D3DXVECTOR2(1, 1);
		vertices[3].Uv = D3DXVECTOR2(1, 1);
		vertices[4].Uv = D3DXVECTOR2(0, 0);
		vertices[5].Uv = D3DXVECTOR2(1, 0);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}

	D3DXMatrixLookAtLH(&view,
		&D3DXVECTOR3(0, 0, -1), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));

	worldVariable = effect->AsMatrix("World");
	viewVariable = effect->AsMatrix("View");
	projVariable = effect->AsMatrix("Projection");
	mapVariable = effect->AsSRV("Map");

	viewVariable->SetMatrix(view);
	projVariable->SetMatrix(orthoGraphic);

	UpdateWorld();
}

Render2D::~Render2D()
{
	SAFE_DELETE(effect);

	SAFE_RELEASE(vertexBuffer);
}

void Render2D::Position(float x, float y)
{
	Position(D3DXVECTOR2(x, y));
}

void Render2D::Position(D3DXVECTOR2 & vec)
{
	position = vec;

	UpdateWorld();
}

void Render2D::Position(D3DXVECTOR2 * vec)
{
	*vec = position;
}

void Render2D::Scale(float x, float y)
{
	Scale(D3DXVECTOR2(x, y));
}

void Render2D::Scale(D3DXVECTOR2 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Render2D::Scale(D3DXVECTOR2 * vec)
{
	*vec = scale;
}

void Render2D::Update()
{

}

void Render2D::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	effect->Draw(0, 0, 6);
}

void Render2D::SRV(ID3D11ShaderResourceView * srv)
{
	mapVariable->SetResource(srv);
}

Effect * Render2D::GetEffect()
{
	return effect;
}

void Render2D::UpdateWorld()
{
	D3DXMATRIX S, T, W;
	D3DXMatrixScaling(&S, scale.x, scale.y, 1);
	// 위치 보정하려고 scale * 0.5f 한거
	D3DXMatrixTranslation(&T,
		position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, 0);

	W = S * T;

	worldVariable->SetMatrix(W);
}
