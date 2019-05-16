#pragma once
#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad(Material* material, float width = 1.0f, float height = 1.0f, Effect*lineShader = NULL);
	~MeshQuad();

	void CreateData() override;
	virtual void ColliderUpdate();

private:
	float width;
	float height;
};