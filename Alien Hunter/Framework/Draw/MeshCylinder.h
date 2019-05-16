#pragma once
#include "Mesh.h"

class MeshCylinder : public Mesh
{
public:
	MeshCylinder(Material* material, float topRadius, float bottomRadius, float height, UINT sliceCount = 10, UINT stackCount = 10, Effect*lineShader = NULL);
	~MeshCylinder();

	void CreateData() override;
	virtual void ColliderUpdate();
private:
	void BuildTopCap(vector<VertexTextureNormalTangent>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<VertexTextureNormalTangent>& vertices, vector<UINT>& indices);

private:
	float topRadius;
	float bottomRadius;
	float height;

	UINT sliceCount;
	UINT stackCount;
};