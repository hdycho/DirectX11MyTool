#pragma once

class GameRender
{
public:
	GameRender();
	virtual ~GameRender();

	void Enable(bool val);
	bool Enable();
	vector<function<void(bool)>> Enabled;

	void Visible(bool val);
	bool Visible();
	vector<function<void(bool)>> Visibled;

	void RootAxis(D3DXMATRIX& matrix);
	void RootAxis(D3DXVECTOR3& rotation);
	D3DXMATRIX RootAxis();

	D3DXMATRIX World();
	void World(D3DXMATRIX matrix);

	void Position(D3DXVECTOR3& vec);
	void Position(float x, float y, float z);
	void PositionInit(float x, float y, float z);
	D3DXVECTOR3&Position();

	virtual void Scale(D3DXVECTOR3& vec);
	virtual void Scale(float x, float y, float z);
	virtual D3DXVECTOR3&Scale();
	virtual D3DXMATRIX ScaleMatrix() { return D3DXMATRIX(); }

	void Rotation(D3DXVECTOR3& vec);
	void Rotation(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(float x, float y, float z);
	D3DXVECTOR3&Rotation();
	D3DXVECTOR3 RotationDegree();

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	D3DXMATRIX Transformed();

	virtual void Update();
	virtual void Render();
	virtual void PostRender();

	void UpdateWorld();
protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;

private:
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};