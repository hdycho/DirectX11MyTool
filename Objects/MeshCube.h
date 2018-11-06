#pragma once
#include "GameModel.h"

class MeshCube :public GameModel
{
public:
	MeshCube(ExecuteValues * values);
	~MeshCube();

	void Update() override;
	void Render() override;

	bool IsAttacked(D3DXVECTOR3 startPos,D3DXVECTOR3 dir);
	void InputPlayerPtr(class Player*p) { player = p; }
private:
	vector<wstring> selectColliders;
	wstring colliderName;

	class Player*player;

	int hp;
};