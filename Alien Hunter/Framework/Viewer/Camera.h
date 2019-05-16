#pragma once

class Camera
{
public:
	Camera(); // �ڽĿ����� ���� �� �հ� protected��
	virtual ~Camera(); // ����� ����ϹǷ�

	virtual void Update() = 0; // ���� �����Լ� ����� �߻� Ŭ������ ����

	void Position(D3DXVECTOR3* vec)
	{
		*vec = position;
	}

	D3DXMATRIX&GetRotationMat() { return matRotation; }

	void Position(float x, float y, float z)
	{
		position = D3DXVECTOR3(x, y, z);
		View();
	}

	void Rotation(D3DXVECTOR2* vec)
	{
		*vec = rotation;
	}

	void RotationDegree(D3DXVECTOR2* vec)
	{
		*vec = rotation * 180.0f / (float)D3DX_PI;
	}

	void Rotation(float x, float y)
	{
		rotation = D3DXVECTOR2(x, y);

		Rotation();
	}

	void RotationDegree(float x, float y)
	{
		//rotation = D3DXVECTOR2(x, y) * (float)D3DX_PI / 180.0f;
		// 0.01745328f = �̰� 1 ���� �� �׳� �ǹ����� �̷��� ��
		rotation = D3DXVECTOR2(x, y) * 0.01745328f;
		Rotation();
	}

	void Matrix(D3DXMATRIX* view)
	{
		*view = matView;
	}

	void Forward(D3DXVECTOR3* vec)
	{
		*vec = forward;
	}

	void Right(D3DXVECTOR3* vec)
	{
		*vec = right;
	}

	void Up(D3DXVECTOR3* vec)
	{
		*vec = up;
	}

	D3DXVECTOR3 Direction(Viewport * vp, Perspective * perspective);

protected:
	virtual void Move();
	virtual void Rotation();
	virtual void View();

private:
	D3DXVECTOR3 position;
	D3DXVECTOR2 rotation; // z ȸ���� �ʿ����

	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	D3DXMATRIX matRotation;
	D3DXMATRIX matView; // ���������� ���̴��� �� ���
};