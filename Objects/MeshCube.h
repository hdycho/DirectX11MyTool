#pragma once
#include "GameModel.h"

class MeshCube :public GameModel
{
public:
	MeshCube(ExecuteValues * values);
	~MeshCube();

	void Update() override;
	void Render() override;
private:
	vector<wstring> selectColliders;
	wstring colliderName;
};