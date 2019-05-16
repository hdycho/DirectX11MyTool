#pragma once

class IObject
{
public:
	virtual bool IsPicked() = 0;
	virtual void SetPickState(bool val) = 0;

	virtual void Position(float x, float y, float z)=0;
	virtual void Position(D3DXVECTOR3& vec)=0;
	virtual void Position(D3DXVECTOR3* vec)=0;

	virtual void Rotation(float x, float y, float z)=0;
	virtual void Rotation(D3DXVECTOR3& vec)=0;
	virtual void Rotation(D3DXVECTOR3* vec)=0;

	virtual void RotationDegree(float x, float y, float z)=0;
	virtual void RotationDegree(D3DXVECTOR3& vec)=0;
	virtual void RotationDegree(D3DXVECTOR3* vec)=0;

	virtual void Scale(float x, float y, float z)=0;
	virtual void Scale(D3DXVECTOR3& vec)=0;
	virtual void Scale(D3DXVECTOR3* vec)=0;

	virtual void Matrix(D3DXMATRIX* mat)=0;

	virtual D3DXVECTOR3 Direction()=0;
	virtual D3DXVECTOR3 Up()=0;
	virtual D3DXVECTOR3 Right()=0;
};
