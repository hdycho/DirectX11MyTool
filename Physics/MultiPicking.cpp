#include "stdafx.h"
#include "MultiPicking.h"
#include "./DebugDraw/DebugRect.h"

MultiPicking::MultiPicking(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	:values(values), zFar(zFar), camera(camera), perspective(perspective)
{
	if (camera == NULL)
		this->camera = values->MainCamera;

	if (perspective == NULL)
		this->perspective = values->Perspective;

	for (int i = 0; i < 4; i++)
		crossPoint[i] = { 0,0,0 };

	for (int i = 0; i < 6; i++)
		planes[i] = D3DXPLANE(0, 0, 5000.0f, 0.0f);

	startPos = { 0,0,0 };
	endPos = { 0,0,0 };

	draw = new DebugRect(DebugRect::POINT, crossPoint);
}

MultiPicking::~MultiPicking()
{
}

void MultiPicking::Update()
{
	D3DXMATRIX V, P;
	camera->Matrix(&V);
	perspective->GetMatrix(&P);

	float zNear = -P._43 / P._33;
	float r = zFar / (zFar - zNear);

	P._33 = r;
	P._43 = -zNear*r;

	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &V, &P);

	//   Near
	planes[0].a = matrix._14 + matrix._13;
	planes[0].b = matrix._24 + matrix._23;
	planes[0].c = matrix._34 + matrix._33;
	planes[0].d = matrix._44 + matrix._43;

	//Far(원면)
	planes[1].a = matrix._14 - matrix._13;
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;

	//마우스좌표 얻기
	DrawRect();
}

void MultiPicking::Render()
{
	draw->Render();
	ImGui::Text("%.2f", crossPoint[0].x);
	ImGui::Text("%.2f", crossPoint[2].x);
}

bool MultiPicking::ContainPont(D3DXVECTOR3 & position)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}
	return true;
}

void MultiPicking::DrawRect()
{
	mouse[0] = { 0,0,0 };
	if (Keyboard::Get()->Press(VK_SPACE))
	{
		if (Mouse::Get()->Down(0)) // 맨처음 누른곳이 시작위치
		{
			startPos = Mouse::Get()->GetPosition();
		}

		//누르고있을때 마우스포지션계속받아오기
		if (Mouse::Get()->Press(0))
		{
			endPos = Mouse::Get()->GetPosition();
			camPosition = camera->Position();

			mouse[0] = { startPos.x,startPos.y ,0 };
			mouse[1] = { endPos.x,startPos.y ,0 };
			mouse[2] = { endPos.x,endPos.y ,0 };
			mouse[3] = { startPos.x,endPos.y ,0 };

			for (int i = 0; i < 4; i++)
			{
				GetDirection(mouse[i], direction[i]);
				GetCrossPoint(camPosition, direction[i], crossPoint[i]);
			}

			draw->ChangeRectPoint(crossPoint);
			draw->ChangeColor(D3DXCOLOR(1, 0, 0, 1));
		}

		if (Mouse::Get()->Up(0)) // 때면 나머지 위치 처리
		{
			if ((int)crossPoint[0].x == (int)crossPoint[2].x) return;
			SettingPlane(camPosition);
		}
	}
}

void MultiPicking::GetDirection(D3DXVECTOR3 & targetPos, OUT D3DXVECTOR3&dir)
{
	dir = camera->Direction(targetPos, values->Viewport, perspective);
	D3DXVec3Normalize(&dir, &dir);
}

void MultiPicking::GetCrossPoint(D3DXVECTOR3 camPos,D3DXVECTOR3 direction,OUT D3DXVECTOR3&crossPos)
{
	float u = (planes[1].a*camPos.x+ planes[1].b*camPos.y + planes[1].c*camPos.z + planes[1].d/
		((planes[1].a*-direction.x)+(planes[1].b*-direction.y)+(planes[1].c*-direction.z)));


	// P=P1+u(P2-P1) 직교점 공식
	crossPos = camPos + u*direction;
}

void MultiPicking::SettingPlane(D3DXVECTOR3 camPos)
{
	D3DXPlaneFromPoints(&planes[2], &camPos, &crossPoint[1], &crossPoint[0]);
	D3DXPlaneFromPoints(&planes[3], &camPos, &crossPoint[2], &crossPoint[1]);
	D3DXPlaneFromPoints(&planes[4], &camPos, &crossPoint[3], &crossPoint[2]);
	D3DXPlaneFromPoints(&planes[5], &camPos, &crossPoint[0], &crossPoint[3]);
}
