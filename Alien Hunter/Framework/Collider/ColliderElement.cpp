#include "Framework.h"
#include "ColliderElement.h"
#include "CollisionContext.h"
#include "Draw\DebugLine.h"

ColliderElement::ColliderElement(Effect*lineShader)
	: name(L""), type(-1), parentLayer(NULL), isShowLine(true), lineShader(lineShader)
{
	D3DXMatrixIdentity(&transform);

	line = NULL;

	if (this->lineShader == NULL)
	{
		isShowLine = false;
	}
	else
	{
		line = new DebugLine(this->lineShader);
		line->Color(0, 1, 0);
		isShowLine = true;
	}

	position = { 0,0,0 };
	scale = { 1,1,1 };
	rotation = { 0,0,0 };
}

ColliderElement::ColliderElement(ColliderElement & copy)
{
	name = copy.name;
	type = copy.type;
	parentLayer = copy.parentLayer;
	isShowLine = copy.isShowLine;
	lineShader = copy.lineShader;

	if (lineShader == NULL)
	{
		isShowLine = false;
	}
	else
	{
		line = new DebugLine(lineShader);
		line->Color(0, 1, 0);
		isShowLine = true;
	}

	transform = copy.transform;
	setBox.assign(copy.setBox.begin(), copy.setBox.end());

	position = copy.position;
	scale = copy.scale;
	rotation = copy.rotation;
}

ColliderElement::~ColliderElement()
{
	SAFE_DELETE(line);
}

void ColliderElement::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

void ColliderElement::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	UpdateWorld();
}

void ColliderElement::Position(D3DXVECTOR3 * vec)
{
	*vec = position;
}

void ColliderElement::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void ColliderElement::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void ColliderElement::Rotation(D3DXVECTOR3 * vec)
{
	*vec = rotation;
}

void ColliderElement::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

void ColliderElement::RotationDegree(D3DXVECTOR3 & vec)
{
	rotation = vec * Math::PI / 180.0f;

	UpdateWorld();
}

void ColliderElement::RotationDegree(D3DXVECTOR3 * vec)
{
	*vec = rotation * 180.0f / Math::PI;
}

void ColliderElement::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

void ColliderElement::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void ColliderElement::Scale(D3DXVECTOR3 * vec)
{
	*vec = scale;
}

void ColliderElement::RemoveInLayer()
{
	if (parentLayer != NULL)
	{
		parentLayer->RemoveCollider(this);
		parentLayer = NULL;
	}
}

void ColliderElement::DrawCollider()
{
	if (isShowLine)
		line->Render();
}

void ColliderElement::Ready()
{
	if (isShowLine)
		line->Ready();
}

void ColliderElement::CreateDebugLine(Effect * lineShader)
{
	line = new DebugLine(lineShader);
	line->Ready();
	line->Color(0, 1, 0);
	isShowLine = true;
}

void ColliderElement::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(
		&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	transform = S * R * T;
}

void ColliderElement::Name(wstring val)
{
	name = val;
}

wstring ColliderElement::Name()
{
	return name;
}

void ColliderElement::Type(UINT val)
{
	type = val;
}

UINT ColliderElement::Type()
{
	return type;
}
