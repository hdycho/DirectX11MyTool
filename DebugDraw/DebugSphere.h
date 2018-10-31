#pragma once
#include "DebugObj.h"

class DebugSphere :public DebugObj
{
private:
	UINT width, height;
	D3DXMATRIX sphereWorld;
public:
	DebugSphere(float radius = 1, D3DXVECTOR3 pos = { 0,0,0 }, D3DXVECTOR3 sacle = { 1,1,1 });
	~DebugSphere();

	void ChangeColor(D3DXCOLOR color);
	void Update(D3DXMATRIX parentMatrix) override;
	void Render() override;

	inline D3DXCOLOR&GetColor() { return colorBuffer->Data.color; }
};
