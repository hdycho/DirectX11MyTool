#pragma once
#include "Mesh.h"

class MeshCube : public Mesh
{
public:
	MeshCube(Material* material, float width, float height, float depth, Effect*lineShader = NULL);
	~MeshCube();

	void CreateData() override;
	virtual void ColliderUpdate();

private:
	float width;
	float height;
	float depth;
};