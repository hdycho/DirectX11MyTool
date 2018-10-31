#include "stdafx.h"
#include "Frustum.h"
#include "./Physics/LineMaker.h"

Frustum::Frustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	:values(values), zFar(zFar), camera(camera), perspective(perspective)
{
	if (camera == NULL)
		this->camera = values->MainCamera;

	if (perspective == NULL)
		this->perspective = values->Perspective;

	for (int i = 0; i < 12; i++)
	{
		line[i] = new LineMaker;
		line[i]->SizeInit(0);
	}

	pWidth = perspective->GetWidth();
	pHeight = perspective->GetHeight();
	isShowFrustum = false;
}

Frustum::~Frustum()
{

}

void Frustum::Update()
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

	//   Far
	planes[1].a = matrix._14 - matrix._13;
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;

	//   Left
	planes[2].a = matrix._14 + matrix._11;
	planes[2].b = matrix._24 + matrix._21;
	planes[2].c = matrix._34 + matrix._31;
	planes[2].d = matrix._44 + matrix._41;

	//   Right
	planes[3].a = matrix._14 - matrix._11;
	planes[3].b = matrix._24 - matrix._21;
	planes[3].c = matrix._34 - matrix._31;
	planes[3].d = matrix._44 - matrix._41;

	//   Top
	planes[4].a = matrix._14 - matrix._12;
	planes[4].b = matrix._24 - matrix._22;
	planes[4].c = matrix._34 - matrix._32;
	planes[4].d = matrix._44 - matrix._42;

	//   Bottom
	planes[5].a = matrix._14 + matrix._12;
	planes[5].b = matrix._24 + matrix._22;
	planes[5].c = matrix._34 + matrix._32;
	planes[5].d = matrix._44 + matrix._42;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);

	if (isShowFrustum)
		CalFrustumLine();
}

void Frustum::Render()
{
	if (isShowFrustum)
	{
		for (int i = 0; i < 8; i++)
		{
			line[i]->DrawLine();
		}
	}
}

void Frustum::ImguiRender()
{
	if (ImGui::Button("Show Frustum"))
	{
		if (!isShowFrustum)
			isShowFrustum = true;
		else
			isShowFrustum = false;
	}

	ImGui::Text("Frustum Width");
	ImGui::SliderFloat("width", &pWidth, 400, 1280);
	ImGui::Text("Frustum Height");
	ImGui::SliderFloat("height", &pHeight, 300, 720);
	perspective->Set(pWidth, pHeight);
}

bool Frustum::ContainPont(D3DXVECTOR3 & position)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}

bool Frustum::ContainCube(D3DXVECTOR3 & center, float radius)
{
	D3DXVECTOR3 check;
	for (int i = 0; i < 6; i++)
	{
		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

void Frustum::CalFrustumLine()
{
	D3DXVECTOR3 camDir = camera->Forward();
	D3DXVECTOR3 camPos = camera->Position();

	//화면상 width와 height를 이용하여 4개의 2차원 좌표를 구한다
	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3DXVECTOR3 startPos, endPos;
	startPos = { desc.Width / 2 - pWidth / 2, desc.Height / 2 - pHeight / 2, 0 };
	endPos = { desc.Width / 2 + pWidth / 2, desc.Height / 2 + pHeight / 2, 0 };

	points[0] = D3DXVECTOR3(startPos.x, startPos.y, 0);
	points[1] = D3DXVECTOR3(endPos.x, startPos.y, 0);
	points[2] = D3DXVECTOR3(endPos.x, endPos.y, 0);
	points[3] = D3DXVECTOR3(startPos.x, endPos.y, 0);

	//카메라부터 각 2차원좌표의 방향을 구한다
	for (int i = 0; i < 4; i++)
		direction[i] = camera->Direction(points[i], values->Viewport, values->Perspective);

	//카메라의 위치와 방향이 나왔으니 교점을 구한다
	for (int i = 0; i < 4; i++)
	{
		//평면과 직선의 교점(원면)
		//P=P0+u*(p2-p1)
		float u = (D3DXPlaneDotCoord(&planes[1], &camPos) /
			((planes[1].a*-direction[i].x) + (planes[1].b*-direction[i].y) + (planes[1].c*-direction[i].z)));
		farPoint[i] = camPos + u*direction[i];

		line[i]->UpdateLine(camPos, farPoint[i]);
	}
	line[4]->UpdateLine(farPoint[0], farPoint[1]);
	line[5]->UpdateLine(farPoint[1], farPoint[2]);
	line[6]->UpdateLine(farPoint[2], farPoint[3]);
	line[7]->UpdateLine(farPoint[3], farPoint[0]);
}
