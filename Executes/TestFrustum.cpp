#include "stdafx.h"
#include "TestFrustum.h"
#include "../LandScape/Terrain.h"
#include "../Viewer/Fixity.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"
#include "../Viewer/Frustum.h"
#include "../Physics/MultiPicking.h"

TestFrustum::TestFrustum(ExecuteValues * values)
	: Execute(values), drawCount(0)
{
	
	Shader*shader = new Shader(Shaders + L"022_DiffuseNone.hlsl");
	
	camera = new Fixity();
	camera->Position(0, 0, -50);
	frusthum = new Frustum(values,1000);

	for (float z = -50; z < 50; z += 10)
	{
		for (float y = -50; y < 50; y += 10)
		{
			for (float x = -50; x < 50; x += 10)
			{
				positions.push_back(D3DXVECTOR3(x, y, z));
			}
		}
	}

	for (int i = 0; i < 500; i++)
	{
		MeshSphere*sphere = new MeshSphere(values);
		sphere->SetShader(shader);
		sphere->SetDiffuse(1, 1, 1, 1);
		sphere->Position(positions[i]);
		spheres.push_back(sphere);
	}

	mk = new MultiPicking(values,1000);
}

TestFrustum::~TestFrustum()
{

}

void TestFrustum::Update()
{
	frusthum->Update();
	mk->Update();
	for (MeshSphere*sphere : spheres)
		sphere->Update();
	//plane->Update();
}

void TestFrustum::Render()
{
	drawCount = 0;
	mk->Render();
	/*for (MeshSphere* sphere : spheres)
	{
		if (frusthum->ContainPont(sphere->Position())==true)
		{
			if (mk->ContainPont(sphere->Position()) == true)
			{
				sphere->GetCollider()->PickingState() = true;
				drawCount++;
			}
			else
				sphere->GetCollider()->PickingState() = false;
			sphere->Render();
		}
	}*/
	ImGui::Text("Draw %d/%d", drawCount,positions.size());
}

