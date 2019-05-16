#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	D3DXVECTOR3 forward, right, up;
	
	Forward(&forward);
	Right(&right);
	Up(&up);

	// �̵�ó��
	{
		D3DXVECTOR3 position;
		Position(&position);

		if (Mouse::Get()->Press(1))
		{
			if (Keyboard::Get()->Press('T'))
				position += forward * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('G'))
				position += -forward * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('F'))
				position += -right * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('H'))
				position += right * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('E'))
				position += up * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('Q'))
				position += -up * moveSpeed * Time::Delta();

			Position(position.x, position.y, position.z);
		}
	}

	// ȸ�� ó��
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		// 0 ���� 1 ������ ��ư 2 ��� ��ư
		if (Mouse::Get()->Press(1)) {
			// ���콺 �̵��� �������°� �󸶸�ŭ �̵��ߴ����� ����
			D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

			// ���콺 ���Ʒ��� �����̸� x�� ȸ���̶� y�ΰ�
			rotation.x += val.y * rotationSpeed * Time::Delta();
			// �¿�ȸ���� y�� ȸ��
			rotation.y += val.x * rotationSpeed * Time::Delta();
		}

		Rotation(rotation.x, rotation.y);
	}
}
