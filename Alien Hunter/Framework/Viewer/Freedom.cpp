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

	// 이동처리
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

	// 회전 처리
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		// 0 왼쪽 1 오른쪽 버튼 2 가운데 버튼
		if (Mouse::Get()->Press(1)) {
			// 마우스 이동값 가져오는거 얼마만큼 이동했는지의 차값
			D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

			// 마우스 위아래로 움직이면 x축 회전이라 y인거
			rotation.x += val.y * rotationSpeed * Time::Delta();
			// 좌우회전이 y축 회전
			rotation.y += val.x * rotationSpeed * Time::Delta();
		}

		Rotation(rotation.x, rotation.y);
	}
}
