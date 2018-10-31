#pragma once
#include "DebugObj.h"

class DebugCube :public DebugObj
{
private:
	D3DXVECTOR3 centerPos;
	float width, height, depth;

	D3DXMATRIX cubeWorld;
public:
	DebugCube(D3DXVECTOR3 center, float width, float height, float depth);
	~DebugCube();
	void ChangeColor(D3DXCOLOR color);
	void Update(D3DXMATRIX parentMatrix) override;
	void Render() override;

	inline D3DXCOLOR&GetColor() { return colorBuffer->Data.color; }
	inline D3DXVECTOR3&GetPosition() { return centerPos; }
	inline D3DXVECTOR3&GetLength()
	{
		D3DXVECTOR3 length(width, depth, height);
		return length;
	}
};