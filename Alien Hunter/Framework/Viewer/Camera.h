#pragma once

class Camera
{
public:
	Camera(); // 자식에서만 만들 수 잇게 protected로
	virtual ~Camera(); // 상속을 줘야하므로

	virtual void Update() = 0; // 순수 가상함수 만들어 추상 클래스로 만듬

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
		// 0.01745328f = 이게 1 라디안 값 그냥 실무에선 이렇게 씀
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
	D3DXVECTOR2 rotation; // z 회전은 필요없음

	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	D3DXMATRIX matRotation;
	D3DXMATRIX matView; // 최종적으로 셰이더에 들어갈 행렬
};