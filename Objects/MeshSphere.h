#pragma once
#include "GameModel.h"

class MeshSphere :public GameModel
{
public:
	MeshSphere(ExecuteValues * values);
	~MeshSphere();

	void Update() override;
	void Render() override;

private:

};