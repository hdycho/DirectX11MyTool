#include "stdafx.h"
#include "GameRender.h"

GameRender::GameRender()
	: enable(true), visible(true)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	D3DXMatrixIdentity(&rootAxis);
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

void GameRender::RootAxis(D3DXMATRIX & matrix)
{
	rootAxis = matrix;
}

void GameRender::RootAxis(D3DXVECTOR3 & rotation)
{
	D3DXMatrixRotationYawPitchRoll
	(
		&rootAxis
		, Math::ToRadian(rotation.y)
		, Math::ToRadian(rotation.x)
		, Math::ToRadian(rotation.z)
	);
}

D3DXMATRIX GameRender::RootAxis()
{
	return rootAxis;
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::World(D3DXMATRIX matrix)
{
	world = matrix;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
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

void GameRender::PositionInit(float x, float y, float z)
{
	position = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3&GameRender::Position()
{
	return position;
}

void GameRender::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void GameRender::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3&GameRender::Scale()
{
	return scale;
}

void GameRender::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void GameRender::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void GameRender::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(vec);
}

void GameRender::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3&GameRender::Rotation()
{
	return rotation;
}

D3DXVECTOR3 GameRender::RotationDegree()
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
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

D3DXMATRIX GameRender::Transformed()
{
	return rootAxis * world;
}

void GameRender::Update()
{
}

void GameRender::Render()
{
}

void GameRender::PostRender()
{
}

void GameRender::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	//   짐벌락 해결 방법 
	//   1. YawPitchRoll 함번에 처리
	//   2. 사원수 사용(Quaterion) :  
	//   TODO : Quaterion 찾아보기
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);

	direction *= 100;
	up *= 100;
	right *= 100;

	D3DXVec3Normalize(&direction, &direction);
	D3DXVec3Normalize(&up, &up);
	D3DXVec3Normalize(&right, &right);
}