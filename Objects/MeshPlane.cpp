#include "stdafx.h"
#include "MeshPlane.h"
#include "../Physics/ColliderBox.h"

MeshPlane::MeshPlane(ExecuteValues * values)
	:GameModel
	(
		Materials + L"Meshes/", L"Plane.material", 
		Models + L"Meshes/", L"Plane.mesh", values
	)
{
	myPtr = this;
	Collider*collider = new ColliderBox(World(), COLLIDERSTATE::BOX, false, L"PlaneCollider");
	collider->InputColliderInfo(&InitBoxPos(), &CurBoxPos(), &MinmaxPos());
	collider->ColliderLineInit();
	colliders.push_back(collider);
}

MeshPlane::~MeshPlane()
{
	SAFE_DELETE(shader);
}

void MeshPlane::Update()
{
	//슈퍼 : 바로위에 부모 클래스
	__super::Update();
}

void MeshPlane::Render()
{
	__super::Render();
}


