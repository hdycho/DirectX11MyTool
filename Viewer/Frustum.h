#pragma once

class Frustum
{
public:
	Frustum(ExecuteValues* values,
		float zFar,
		Camera* camera = NULL,
		Perspective* perspective = NULL);
	~Frustum();

	void Update();
	void Render();
	void ImguiRender();

	bool ContainPont(D3DXVECTOR3& position);
	bool ContainCube(D3DXVECTOR3&center, float radius);
private:
	void CalFrustumLine();
private:
	ExecuteValues* values;
	D3DXPLANE planes[6];

	float zFar;

	Camera* camera;
	Perspective* perspective;

	///////////프러스텀 그리기//////////
	class LineMaker*line[8];
	D3DXVECTOR3 points[4];
	D3DXVECTOR3 direction[4];
	D3DXVECTOR3 farPoint[4];
	float pWidth, pHeight;
	bool isShowFrustum;
	///////////////////////////////////
};