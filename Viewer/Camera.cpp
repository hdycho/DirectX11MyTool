#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	Move();
	Rotation();
}

Camera::~Camera()
{
}

D3DXVECTOR3 Camera::Direction(Viewport * vp, Perspective * perspective)
{
	D3DXVECTOR2 screenSize;

	//뷰포트의 크기 받아옴 화면크기나옴
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	//마우스 포지션 받아오기 => 2차원화면상의 좌표
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR2 point;

	//Viewport
	{
		//마우스포지션을 0~1사이로 정규화시킴
		point.x = ((2.0f*mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f*mouse.y) / screenSize.y) - 1.0f)*-1.0f;
	}


	//w*v*p 순서로 거꾸로 곱해줘서 3차원 좌표구하기
	//Inv Projection 
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		//프로젝션은 크기(비율)을 늘리는 변환
		//스케일을 곱해주는 변환을 반대로 나눠줌
		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	//Inv View
	{
		D3DXMATRIX invView;
		//카메라의 역행렬 구하기
		D3DXMatrixInverse(&invView, NULL, &matView);

		/*	dir.x = (point.x*invView._11) + (point.y*invView._21) + invView._31;
		dir.y = (point.x*invView._12) + (point.y*invView._22) + invView._32;
		dir.z = (point.x*invView._13) + (point.y*invView._23) + invView._33;*/

		//지금 마우스는 2차원 좌표로 설정되있기 때문에 z는 1로 설정
		//마우스 포지션에 카메라 뷰의 역행렬 곱해주면 뷰변환 전의 과정으로 돌아감
		//월드변환된 방향나옴
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

//멀티피킹시 사용
D3DXVECTOR3 Camera::Direction(D3DXVECTOR3 mpos, Viewport * vp, Perspective * perspective)
{
	D3DXVECTOR2 screenSize;

	//뷰포트의 크기 받아옴 화면크기나옴
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	D3DXVECTOR2 point;

	//Viewport
	{
		//마우스포지션을 0~1사이로 정규화시킴
		point.x = ((2.0f*mpos.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f*mpos.y) / screenSize.y) - 1.0f)*-1.0f;
	}


	//w*v*p 순서로 거꾸로 곱해줘서 3차원 좌표구하기
	//Inv Projection 
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		//프로젝션은 크기(비율)을 늘리는 변환
		//스케일을 곱해주는 변환을 반대로 나눠줌
		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	//Inv View
	{
		D3DXMATRIX invView;
		//카메라의 역행렬 구하기
		D3DXMatrixInverse(&invView, NULL, &matView);

		/*	dir.x = (point.x*invView._11) + (point.y*invView._21) + invView._31;
		dir.y = (point.x*invView._12) + (point.y*invView._22) + invView._32;
		dir.z = (point.x*invView._13) + (point.y*invView._23) + invView._33;*/

		//지금 마우스는 2차원 좌표로 설정되있기 때문에 z는 1로 설정
		//마우스 포지션에 카메라 뷰의 역행렬 곱해주면 뷰변환 전의 과정으로 돌아감
		//월드변환된 방향나옴
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x);
	D3DXMatrixRotationY(&y, rotation.y);

	matRotation = x * y;
	
	//	방향 공간 변환
	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
}

void Camera::View()
{
	//	position + forward 자기보다 1만큼 앞을 바라봐라
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
