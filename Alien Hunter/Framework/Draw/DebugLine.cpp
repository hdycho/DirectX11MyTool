#include "Framework.h"
#include "DebugLine.h"
#include "ILine.h"

const UINT DebugLine::MaxCount = 5000;

DebugLine::DebugLine(Effect*effect)
	: color(0, 0, 0, 1)
	, effect(effect)
{
	vertexBuffer = NULL;
	this->effect = effect;

	worldVariable = this->effect->AsMatrix("World");
	colorVariable = this->effect->AsVector("Color");
}

DebugLine::~DebugLine()
{
	//TODO : 밖에서주는쉐이더이면 삭제하면안됨
	//SAFE_DELETE(shader);
	SAFE_RELEASE(vertexBuffer);
}

void DebugLine::Initialize()
{
}

void DebugLine::Ready()
{
	if (effect == NULL)
		effect = new Effect(Effects + L"001_Line.hlsl");

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * MaxCount * 2;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void DebugLine::Draw(D3DXMATRIX& world, ILine * line)
{
	if (vertexBuffer == NULL)
	{
		//Create Vertex Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(Vertex) * MaxCount * 2;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &vertexBuffer);
			assert(SUCCEEDED(hr));
		}
	}
	vector<D3DXVECTOR3> temp;
	line->GetLine(world, temp);
	this->world = world;

	lines.clear();
	for (size_t i = 0; i < temp.size(); i += 2)
	{
		Line line;
		line.start = temp[i + 0];
		line.end = temp[i + 1];
		lines.push_back(line);
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, &lines[0], sizeof(Line) * lines.size());

	D3D::GetDC()->Unmap(vertexBuffer, 0);
}

void DebugLine::UpdateWorld(D3DXMATRIX & world)
{
	this->world = world;
}

void DebugLine::Color(float r, float g, float b)
{
	D3DXCOLOR val(r, g, b, 1.0f);

	Color(val);
}

void DebugLine::Color(D3DXCOLOR & vec)
{
	color = vec;
}

void DebugLine::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorVariable->SetFloatVector(color);

	worldVariable->SetMatrix(world);

	effect->Draw(0, 1, lines.size() * 2);
}
