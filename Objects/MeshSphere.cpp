#include "stdafx.h"
#include "MeshSphere.h"
#include "../Physics/ColliderBox.h"

MeshSphere::MeshSphere(ExecuteValues * values)
	:GameModel
	(
		Materials + L"Meshes/", L"Sphere.material", 
		Models + L"Meshes/", L"Sphere.mesh",values
	)
{

	myPtr = this;
	Collider*collider = new ColliderBox(World(), COLLIDERSTATE::BOX, false, L"SphereCollider");
	collider->InputColliderInfo(&InitBoxPos(), &CurBoxPos(), &MinmaxPos());
	collider->ColliderLineInit();
	colliders.push_back(collider);
}

MeshSphere::~MeshSphere()
{
	SAFE_DELETE(shader);
}

void MeshSphere::Update()
{

	//슈퍼 : 바로위에 부모 클래스
	__super::Update();
}

void MeshSphere::Render()
{
	__super::Render();
}

