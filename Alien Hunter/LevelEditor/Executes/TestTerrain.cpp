#include "stdafx.h"
#include "TestTerrain.h"
#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"
#include "Environment/HeightMap.h"
#include "Collider/ColliderBox.h"
#include "Collider/ColliderRay.h"
#include "Collider/ColliderSphere.h"
#include "Environment\Rain.h"

void TestTerrain::Initialize()
{
	terrainMaterial = new Material(Effects + L"012_Mesh&Model.hlsl");
	cube = new MeshCube(terrainMaterial, 10, 10, 10);
	cube->Name() = L"Cube";
	cube->Position(0, 0, -50);
	cube->RotationDegree(0, 45, 0);
	cube->CreateCollider(0);

	cl = new MeshCylinder(terrainMaterial, 10, 10, 10);
	cl->Name() = L"Cy";
	cl->Position(0, 0, 0);
	cl->CreateCollider(0);

	//player = new ColliderRay(L"Player Collider", D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));
	//player = new ColliderSphere(L"Player Collider", D3DXVECTOR3(0, 0, 0), 0.5f);
	//player = new ColliderBox(L"Player Collider", cube->Min(), cube->Max());


	//enemy = new ColliderRay(L"Enemy Collider", D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0.5f, 0.5f, 0));
	//enemy = new ColliderSphere(L"Enemy Collider", D3DXVECTOR3(0, 0, 0), 0.5f);
	//enemy = new ColliderBox(L"Enemy Collider", D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
}

void TestTerrain::Ready()
{
	//D3DXMATRIX  e;
	//D3DXMatrixTranslation(&e, 60, 0, 0);

	//player->Ready();
	//enemy->Ready();

	//enemy->Transform(e);

	//CollisionContext::Get()->GetLayer(L"Collision Player")->AddCollider(player);
	//CollisionContext::Get()->GetLayer(L"Collision Enemy")->AddCollider(enemy);
}

void TestTerrain::Destroy()
{
	SAFE_DELETE(terrainMaterial);
	//SAFE_DELETE(terrain);

	//SAFE_DELETE(player);
	//SAFE_DELETE(enemy);
}

void TestTerrain::Update()
{
	D3DXVECTOR3 pos;
	cube->Position(&pos);
	pos.z += Time::Delta() * 10;
	cube->Position(pos);

	cube->ColliderUpdate();
	cl->ColliderUpdate();
}

void TestTerrain::PreRender()
{
}

void TestTerrain::Render()
{
	cube->Render();
	cl->Render();
}

void TestTerrain::PostRender()
{
}

void TestTerrain::ResizeScreen()
{
}
