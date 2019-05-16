#pragma once

class BoneSphere :public MeshSphere
{
public:
	BoneSphere(Material* material);
	virtual ~BoneSphere();

	void BoneRender();
	void PostBoneRender();

	void BoneImGuiRender();

	D3DXVECTOR3&InitScale() { return initScale; }
	D3DXVECTOR3&InitPos() { return initPos; }

	D3DXMATRIX GetSetMatrix() { return setMat; }
private:
	void SetMatrixUpdate();
private:
	D3DXCOLOR color;

	D3DXVECTOR3 initScale;
	D3DXVECTOR3 initPos;
	D3DXMATRIX initMat;

private:
	D3DXVECTOR3 setScale;
	D3DXVECTOR3 setRotate;
	D3DXVECTOR3 setPosition;
	D3DXMATRIX setMat;

private:
	ID3DX11EffectVectorVariable*colorVariable;
};
