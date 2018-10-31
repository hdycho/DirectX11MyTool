#pragma once
#include "stdafx.h"

class AABBCollision
{
private:

public:
	static bool IsCollision(vector<D3DXVECTOR3>&model1, vector<D3DXVECTOR3>&model2);
};

bool AABBCollision::IsCollision(vector<D3DXVECTOR3>&model1, vector<D3DXVECTOR3>&model2)
{
	//0번은 최소 1번은 최대점으로 판단
	//1번의 왼쪽충돌
	if (model1[1].x > model2[0].x&&model1[0].x < model2[1].x&&
		model1[0].z<model2[1].z&&model1[1].z>model2[0].z&&
		model1[0].y<model2[1].y&&model1[1].y>model2[0].y)
	{
		return true;
	}
	else
		return false;
}

class OBBCollision
{
private:

public:
	static bool IsCollision(vector<D3DXVECTOR3*>&model1, vector<D3DXVECTOR3*>&model2);
};

bool OBBCollision::IsCollision(vector<D3DXVECTOR3*>&model1, vector<D3DXVECTOR3*>&model2)
{

	return false;
}