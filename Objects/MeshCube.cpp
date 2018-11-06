#include "stdafx.h"
#include "MeshCube.h"
#include "../Physics/ColliderBox.h"
#include "../Objects/Player.h"

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

	hp = 3;
}

MeshCube::~MeshCube()
{
	SAFE_DELETE(shader);
}

void MeshCube::Update()
{
	//슈퍼 : 바로위에 부모 클래스
	__super::Update();

	if (player != NULL)
	{
		if (IsAttacked(player->GetShotStartPos(), player->GetShotDir()))
		{
			SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
			if(player->IsShot())
				hp--;
		}
		else
			SetDiffuse(D3DXCOLOR(1, 1, 1, 1));

		if (hp <= 0)
			isVisible = false;
	}
}

void MeshCube::Render()
{
	__super::Render();
}

bool MeshCube::IsAttacked(D3DXVECTOR3 startPos, D3DXVECTOR3 dir)
{
	vector<UINT> indices;
	indices.resize(36);
	indices = {
		0,1,2,2,1,3,
		2,3,6,6,3,7,
		6,7,4,4,7,5,
		0,1,4,4,1,5,
		1,5,3,3,5,7,
		0,4,2,2,4,6
	};

	for (int i = 0; i < 6; i++)
	{
		D3DXVECTOR3 p[6];
		for (int j = 0; j < 6; j++)
			p[j] = CurBoxPos()[indices[6 * i + j]];

		float u, v, distance;

		if (D3DXIntersectTri(&p[0], &p[1], &p[2], &startPos, &dir, &u, &v, &distance))
		{
			return true;
		}
		else if (D3DXIntersectTri(&p[3], &p[4], &p[5], &startPos, &dir, &u, &v, &distance))
		{
			return true;
		}
	}
	return false;
}


