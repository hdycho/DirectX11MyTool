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

	//����Ʈ�� ũ�� �޾ƿ� ȭ��ũ�⳪��
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	//���콺 ������ �޾ƿ��� => 2����ȭ����� ��ǥ
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR2 point;

	//Viewport
	{
		//���콺�������� 0~1���̷� ����ȭ��Ŵ
		point.x = ((2.0f*mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f*mouse.y) / screenSize.y) - 1.0f)*-1.0f;
	}


	//w*v*p ������ �Ųٷ� �����༭ 3���� ��ǥ���ϱ�
	//Inv Projection 
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		//���������� ũ��(����)�� �ø��� ��ȯ
		//�������� �����ִ� ��ȯ�� �ݴ�� ������
		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	//Inv View
	{
		D3DXMATRIX invView;
		//ī�޶��� ����� ���ϱ�
		D3DXMatrixInverse(&invView, NULL, &matView);

		/*	dir.x = (point.x*invView._11) + (point.y*invView._21) + invView._31;
		dir.y = (point.x*invView._12) + (point.y*invView._22) + invView._32;
		dir.z = (point.x*invView._13) + (point.y*invView._23) + invView._33;*/

		//���� ���콺�� 2���� ��ǥ�� �������ֱ� ������ z�� 1�� ����
		//���콺 �����ǿ� ī�޶� ���� ����� �����ָ� �亯ȯ ���� �������� ���ư�
		//���庯ȯ�� ���⳪��
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

//��Ƽ��ŷ�� ���
D3DXVECTOR3 Camera::Direction(D3DXVECTOR3 mpos, Viewport * vp, Perspective * perspective)
{
	D3DXVECTOR2 screenSize;

	//����Ʈ�� ũ�� �޾ƿ� ȭ��ũ�⳪��
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	D3DXVECTOR2 point;

	//Viewport
	{
		//���콺�������� 0~1���̷� ����ȭ��Ŵ
		point.x = ((2.0f*mpos.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f*mpos.y) / screenSize.y) - 1.0f)*-1.0f;
	}


	//w*v*p ������ �Ųٷ� �����༭ 3���� ��ǥ���ϱ�
	//Inv Projection 
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		//���������� ũ��(����)�� �ø��� ��ȯ
		//�������� �����ִ� ��ȯ�� �ݴ�� ������
		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	//Inv View
	{
		D3DXMATRIX invView;
		//ī�޶��� ����� ���ϱ�
		D3DXMatrixInverse(&invView, NULL, &matView);

		/*	dir.x = (point.x*invView._11) + (point.y*invView._21) + invView._31;
		dir.y = (point.x*invView._12) + (point.y*invView._22) + invView._32;
		dir.z = (point.x*invView._13) + (point.y*invView._23) + invView._33;*/

		//���� ���콺�� 2���� ��ǥ�� �������ֱ� ������ z�� 1�� ����
		//���콺 �����ǿ� ī�޶� ���� ����� �����ָ� �亯ȯ ���� �������� ���ư�
		//���庯ȯ�� ���⳪��
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
	
	//	���� ���� ��ȯ
	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
}

void Camera::View()
{
	//	position + forward �ڱ⺸�� 1��ŭ ���� �ٶ����
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
