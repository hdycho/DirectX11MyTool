#pragma once
#include "DebugObj.h"

class DebugCone :public DebugObj
{
	
public:
	DebugCone(float angle = 0.5f, D3DXVECTOR3 dir = { 0,-1,0 }, D3DXVECTOR3 pos = { 0,0,0 });
	~DebugCone();

	void ChangeConeShape(float angle, D3DXVECTOR3 Dir, D3DXVECTOR3 pos);
	void ChangeColor(D3DXCOLOR color);
	void Update(D3DXMATRIX parentMatrix) override;
	void Render() override;

	inline D3DXCOLOR&GetColor() { return colorBuffer->Data.color; }
};
