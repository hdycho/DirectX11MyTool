#include "stdafx.h"
#include "Projection.h"
#include "../Viewer/Fixity.h"
#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Projection::Projection(ExecuteValues * values)
	:values(values)
{
	shader = new Shader(Shaders + L"042_Projection.hlsl");

	fixity = new Fixity();
	fixity->Position(0, 10, 0);
	fixity->RotationDegree(90, 0);

	//Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		//PI*0.5(수직시야각) => 90도를 주면 화면 원본에 대한 비율이 나옴
		//=>h는 cot(fovY/2)이므로 수직시야각에 90도를 넣으면 cot(45)는 1이
		//나온다
		//정투영
		//너비와 높이는 실제너비와 높이가 아니라 넓이,높이에 대한 비율을
		//만들기 위해서 넣는다
		perspective = new Perspective(1, 1, Math::PI*0.5f, 1, 100);
	}

	vpBuffer = new ViewProjectionBuffer();
}

Projection::~Projection()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(fixity);
	SAFE_DELETE(perspective);

	SAFE_DELETE(vpBuffer);
}

void Projection::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Projection::Update()
{
	for (GameModel*obj : objs)
	{
		obj->SetShader(shader);
		obj->SetNormalMap(Textures + L"Bricks.png");
		obj->Update();
	}


	D3DXMATRIX V, P;

	//고정카메라의 뷰행렬가져와서 셋팅
	fixity->Matrix(&V);
	vpBuffer->SetView(V);

	//위에서 새로만든 프로젝션행렬 가져와서 셋팅
	perspective->GetMatrix(&P);
	vpBuffer->SetProjection(P);
}

void Projection::PreRender()
{
}

void Projection::Render()
{
	D3DXVECTOR3 position;
	fixity->Position(&position);
	ImGui::SliderFloat3("Proj Position", (float*)position, -100, 100);
	fixity->Position(position.x, position.y, position.z);

	D3DXVECTOR2 rotation;
	fixity->RotationDegree(&rotation);
	ImGui::SliderFloat2("Proj rotation", (float*)rotation, -180, 180);
	fixity->RotationDegree(rotation.x, rotation.y);


	vpBuffer->SetVSBuffer(10);

	for (GameModel*obj : objs)
	{
		obj->Render();
	}

}
