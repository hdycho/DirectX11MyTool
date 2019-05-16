#pragma once
#include "Mesh.h"

class MeshSphere : public Mesh
{
public:
	MeshSphere(Material* material, float radius, UINT stackCount = 10, UINT sliceCount = 10, Effect*lineShader = NULL);
	~MeshSphere();

	void CreateData() override;
	virtual void ColliderUpdate();

private:
	float radius;

	UINT stackCount;
	UINT sliceCount;
};