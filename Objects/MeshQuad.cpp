#include "stdafx.h"
#include "MeshQuad.h"
#include "../Physics/ColliderBox.h"

MeshQuad::MeshQuad(ExecuteValues * values)
	:GameModel
	(
		Materials + L"Meshes/", L"Quad.material",
		Models + L"Meshes/", L"Quad.mesh",values
	)
{
	myPtr = this;
	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FrontCounterClockwise(true);

	Collider*collider = new ColliderBox(World(), COLLIDERSTATE::BOX, false, L"CubeCollider");
	collider->InputColliderInfo(&InitBoxPos(), &CurBoxPos(), &MinmaxPos());
	collider->ColliderLineInit();
	colliders.push_back(collider);

	Scale(10, 10, 10);
	//Rotation(Math::ToRadian(-45), 0, 0);
}

MeshQuad::~MeshQuad()
{
	SAFE_DELETE(shader);
}

void MeshQuad::Update()
{
	//슈퍼 : 바로위에 부모 클래스
	__super::Update();
}

void MeshQuad::Render()
{
	rasterizerState[1]->RSSetState();
	__super::Render();
	rasterizerState[0]->RSSetState();
}


