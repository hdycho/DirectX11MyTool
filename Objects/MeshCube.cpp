#include "stdafx.h"
#include "MeshCube.h"
#include "../Physics/ColliderBox.h"

MeshCube::MeshCube(ExecuteValues * values)
	:GameModel
	(
		Materials + L"Meshes/", L"Cube.material",
		Models + L"Meshes/", L"Cube.mesh",values
	)
{
	myPtr = this;
	Collider*collider = new ColliderBox(World(), COLLIDERSTATE::BOX, false, L"CubeCollider");
	collider->InputColliderInfo(&InitBoxPos(), &CurBoxPos(), &MinmaxPos());
	collider->ColliderLineInit();
	colliders.push_back(collider);
}

MeshCube::~MeshCube()
{
	SAFE_DELETE(shader);
}

void MeshCube::Update()
{
	//���� : �ٷ����� �θ� Ŭ����
	__super::Update();
}

void MeshCube::Render()
{
	__super::Render();
}


