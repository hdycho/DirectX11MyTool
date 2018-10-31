#pragma once
#include "Execute.h"

class TestFrustum : public Execute
{
public:
	TestFrustum(ExecuteValues* values);
	~TestFrustum();

	void Update();
	void PreRender(){}
	void Render();
	void PostRender() {}
	void ResizeScreen() {}

private:
	UINT drawCount;
	vector<D3DXVECTOR3> positions;

	class MeshPlane*plane;
	vector<class MeshSphere*> spheres;

	class Frustum*frusthum;

	class Camera*camera;
	class Perspective*projection;

	class MultiPicking*mk;
};


