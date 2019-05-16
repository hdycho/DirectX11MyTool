#pragma once
#include "Interface\IObject.h"
#include "Interface\IData.h"
#include "Interface\ICollider.h"
#include "Interface\IShadow.h"

class GameRender :
	public IObject,
	public IData,
	public ICollider,
	public IShadow
{
public:
	GameRender();
	virtual ~GameRender();

	void Enable(bool val);
	bool Enable();
	vector<function<void(bool)>> Enabled;

	void Visible(bool val);
	bool Visible();
	vector<function<void(bool)>> Visibled;

	D3DXMATRIX World();

	virtual void Update() = 0;
	virtual void Render() = 0;
public:
	//IObject를 상속
	virtual bool IsPicked() { return false; }
	virtual void SetPickState(bool val) {}

	void Position(float x, float y, float z);
	void Position(D3DXVECTOR3& vec);
	void Position(D3DXVECTOR3* vec);
	D3DXVECTOR3&GetPosition() { return position; }

	void Rotation(float x, float y, float z);
	void Rotation(D3DXVECTOR3& vec);
	void Rotation(D3DXVECTOR3* vec);
	D3DXVECTOR3&GetRotation() { return rotation; }

	void RotationDegree(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(D3DXVECTOR3* vec);

	void Scale(float x, float y, float z);
	void Scale(D3DXVECTOR3& vec);
	void Scale(D3DXVECTOR3* vec);

	void Matrix(D3DXMATRIX* mat);

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	//ICollider를 통해 상속됨
	virtual vector<ColliderElement*>&GetColliders() { return temp; }
	virtual void CreateCollider(int type) {}
	virtual void ColliderUpdate() {}

	//IShadow를 통해 상속됨
	virtual void SetShader(D3DXMATRIX v, D3DXMATRIX p) {}
	virtual void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv) {}
	virtual void ShadowUpdate() {}
	virtual void NormalRender() {}
	virtual void ShadowRender(UINT tech, UINT pass) {}

public:
	//IData를 상속
	virtual class GData* Save() { return NULL; }
	wstring&LoadDataFile() { return loadFileName; }
	virtual void Load(wstring fileName) {}
	wstring&Name() { return name; } // 저장되는이름
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }

protected:
	virtual void UpdateWorld();

protected:
	bool enable;
	bool visible;
	bool isPicked;

private:
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

	wstring name;
	wstring loadFileName;
	D3DXVECTOR3 loadPosition = {0,0,0};
	D3DXVECTOR3 loadScale = { 1,1,1 };
	D3DXVECTOR3 loadRotate = { 0,0,0 };

	//안씀 게임모델꺼 쓴다
	vector<ColliderElement*> temp;
};