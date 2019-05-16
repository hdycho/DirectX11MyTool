#pragma once
#include "Interface/IShadow.h"
#include "Interface/IData.h"
#include "Interface/IObject.h"
#include "Interface/ICollider.h"

class Mesh :
	public IShadow,
	public IData,
	public IObject,
	public ICollider
{
public:
	Mesh(Material* material, Effect*lineEffect = NULL);
	virtual ~Mesh();

	void Render();

	void RenderShadow(UINT tech, UINT pass);

	void Pass(UINT val) { pass = val; }
	UINT Pass() { return pass; }

	Material * GetMaterial() { return material; }

public:
	//ICollider를 통해 상속됨
	virtual vector<ColliderElement*>&GetColliders() { return colliders; }
	virtual void CreateCollider(int type);
	virtual void ColliderUpdate();

	//IOjbect를 통해 상속됨
	void Position(float x, float y, float z);
	void Position(D3DXVECTOR3& vec);
	void Position(D3DXVECTOR3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(D3DXVECTOR3& vec);
	void Rotation(D3DXVECTOR3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(D3DXVECTOR3* vec);

	void Scale(float x, float y, float z);
	void Scale(D3DXVECTOR3& vec);
	void Scale(D3DXVECTOR3* vec);

	void Matrix(D3DXMATRIX* mat);

	D3DXVECTOR3 Direction() { return direction; }
	D3DXVECTOR3 Up() { return up; }
	D3DXVECTOR3 Right() { return right; }

	bool IsPicked();
	void SetPickState(bool val) { isPicked = val; }
	void PickUpdate();

	// IShadow을(를) 통해 상속됨
	void SetShader(D3DXMATRIX v, D3DXMATRIX p) override;
	void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv) override;
	void ShadowUpdate() override;
	void NormalRender() override;
	void ShadowRender(UINT tech, UINT pass) override;

	//IData를 통해 상속됨
	class GData* Save() { return NULL; }
	wstring&Name() { return meshName; }
	wstring&LoadDataFile() { return loadFile; }
	void Load(wstring fileName) {}
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }


protected:
	virtual void CreateData() = 0;
	void CreateBuffer();

private:
	void UpdateWorld();

protected:
	VertexTextureNormalTangent * vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
	D3DXVECTOR3 min, max;

	vector<ColliderElement*> colliders;
	D3DXMATRIX world;

	vector<D3DXVECTOR3> pickCube;

	Material* material;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

public:
	D3DXVECTOR3&Min() { return min; }
	D3DXVECTOR3&Max() { return max; }

private:
	UINT		pass;
	wstring meshName;

	bool isPicked;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

	Effect*lineEffect;

	//일단 안씀
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
	wstring loadFile;
};