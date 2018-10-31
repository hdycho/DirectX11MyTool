#pragma once

class MultiPicking
{
public:
	MultiPicking(ExecuteValues* values,
		float zFar,
		Camera* camera = NULL,
		Perspective* perspective = NULL);
	~MultiPicking();

	void Update();
	void Render();
	bool ContainPont(D3DXVECTOR3& position);
	void DrawRect();
	void GetDirection(D3DXVECTOR3&targetPos,OUT D3DXVECTOR3&dir);
	void GetCrossPoint(D3DXVECTOR3 camPos, D3DXVECTOR3 direction, OUT D3DXVECTOR3&crossPos);
	void SettingPlane(D3DXVECTOR3 camPos);

private:
	class DebugRect*draw;
	ExecuteValues* values;

	D3DXPLANE planes[6];
	float zFar;

	//카메라 포지션
	D3DXVECTOR3 camPosition;

	Camera* camera;
	Perspective* perspective;

	D3DXVECTOR3 startPos, endPos;
	D3DXVECTOR3 mouse[4];
	D3DXVECTOR3 direction[4];
	D3DXVECTOR3 crossPoint[4];
};
