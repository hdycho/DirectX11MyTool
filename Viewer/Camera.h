#pragma once

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void Update() = 0;

	void Position(D3DXVECTOR3* vec)
	{
		*vec = position;
	}

	void Position(float x, float y, float z)
	{
		position = D3DXVECTOR3(x, y, z);
		View();
	}

	D3DXVECTOR3&Position()
	{
		return position;
	}

	D3DXVECTOR2&GetRotation()
	{
		return rotation;
	}

	void Rotation(D3DXVECTOR2* vec)
	{
		*vec = rotation;
	}

	D3DXMATRIX&GetRotationMat() {return matRotation;}

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

	D3DXVECTOR3 Forward()
	{
		return forward;
	}

	void Right(D3DXVECTOR3* vec)
	{
		*vec = right;
	}

	void Up(D3DXVECTOR3* vec)
	{
		*vec = up;
	}

	D3DXVECTOR3 Direction(
		class Viewport*vp,
		class Perspective*perspective);

	D3DXVECTOR3 Direction(
		D3DXVECTOR3 mpos,
		class Viewport*vp,
		class Perspective*perspective);
protected:
	virtual void Move();
	virtual void Rotation();
	virtual void View();

	

private:
	D3DXVECTOR3 position;
	D3DXVECTOR2 rotation;

	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	D3DXMATRIX matRotation;
	D3DXMATRIX matView;
};