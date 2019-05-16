#include "Framework.h"
#include "GameRender.h"

GameRender::GameRender()
	: enable(true), visible(true), isPicked(false)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	D3DXMatrixIdentity(&world);
}

GameRender::~GameRender()
{

}

void GameRender::Enable(bool val)
{
	enable = val;

	for (auto temp : Enabled)
		temp(val);
}

bool GameRender::Enable()
{
	return enable;
}

void GameRender::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp(val);
}

bool GameRender::Visible()
{
	return visible;
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::Position(D3DXVECTOR3& vec)
{
	position = vec;

	UpdateWorld();
}

void GameRender::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

void GameRender::Position(D3DXVECTOR3* vec)
{
	*vec = position;
}

void GameRender::Scale(D3DXVECTOR3& vec)
{
	scale = vec;

	UpdateWorld();
}

void GameRender::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

void GameRender::Scale(D3DXVECTOR3* vec)
{
	*vec = scale;
}

void GameRender::Matrix(D3DXMATRIX * mat)
{
	*mat = world;
}

void GameRender::Rotation(D3DXVECTOR3& vec)
{
	rotation = vec;

	UpdateWorld();
}

void GameRender::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void GameRender::Rotation(D3DXVECTOR3* vec)
{
	*vec = rotation;
}

void GameRender::RotationDegree(D3DXVECTOR3& vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void GameRender::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}


void GameRender::RotationDegree(D3DXVECTOR3* vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	*vec = temp;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

void GameRender::UpdateWorld()
{
	D3DXMATRIX s,S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixScaling(&s, 0.01f, 0.01f, 0.01f);
	D3DXMatrixRotationYawPitchRoll(
		&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = s*S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);

	D3DXVec3Normalize(&direction, &direction);
	D3DXVec3Normalize(&up, &up);
	D3DXVec3Normalize(&right, &right);
}
