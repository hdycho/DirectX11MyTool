#pragma once
#include "GameModel.h"

class MeshQuad :public GameModel
{
public:
	MeshQuad(ExecuteValues * values);
	~MeshQuad();

	void Update() override;
	void Render() override;
private:
	RasterizerState*rasterizerState[2];
};