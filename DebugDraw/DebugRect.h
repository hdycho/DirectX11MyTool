#pragma once
#include "DebugObj.h"

class DebugRect :public DebugObj
{
public:
	enum RECTTYPE
	{
		FRONT,
		UP,
		POINT
	};
private:
	RECTTYPE rtype;
public:
	DebugRect(RECTTYPE type, float radius = 1, D3DXVECTOR3 pos = { 0,0,0 }, D3DXVECTOR3 sacle = { 1,1,1 });
	DebugRect(RECTTYPE type, D3DXVECTOR3*pos);
	~DebugRect();

	void ChangeRectPoint(D3DXVECTOR3*pos);
	void ChangeColor(D3DXCOLOR color);
	void Update(D3DXMATRIX parentMatrix) override;
	void Render() override;

	inline D3DXCOLOR&GetColor() { return colorBuffer->Data.color; }
};