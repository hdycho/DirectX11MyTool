#pragma once

class CollisionLayer;

#include "Interface\IType.h"
// ------------------------------------------------------------------------- //
//	It's a basic element of collision.
// ------------------------------------------------------------------------- //
class ColliderElement : public IType
{
public:
	enum class EType
	{
		EBox,
		ESphere,
		ERay,
		EModel,
	};
public:
	ColliderElement(Effect*lineShader = NULL);
	ColliderElement(ColliderElement&copy);
	virtual ~ColliderElement();

	// ------------------------------------------------------------------------- //
	//	Get/Set transform matrix.
	// ------------------------------------------------------------------------- //
	virtual void Transform(D3DXMATRIX&world) = 0;
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
	D3DXMATRIX Transform() { return transform; }

	// ------------------------------------------------------------------------- //
	//	Get/Set Parent layer
	// ------------------------------------------------------------------------- //
	void ParentLayer(CollisionLayer * layer) { parentLayer = layer; }
	CollisionLayer * ParentLayer() { return parentLayer; }

	// ------------------------------------------------------------------------- //
	//	Removes this element in the collison layer.
	// ------------------------------------------------------------------------- //
	void RemoveInLayer();
	bool&ShowLine() { return isShowLine; } // 이거설정안하면 디버그라인 생성(쉐이더)안함

	void DrawCollider();
	void Ready();

	virtual void ImguiRender() = 0;


	class DebugLine*GetLine() { return line; }
	void CreateDebugLine(Effect*lineShader); // 이건 툴에서 로드된 빌보드 설정시 생성하게됨
protected:
	wstring name;
	UINT type;

	D3DXMATRIX transform;
	vector<D3DXVECTOR3> setBox;

	CollisionLayer * parentLayer;
	bool isShowLine;
	class DebugLine*line;

	Effect*lineShader;
private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

private:
	void UpdateWorld();

public:
	void Name(wstring val);
	wstring Name();

	// IType을(를) 통해 상속됨
	void Type(UINT val) override;
	UINT Type() override;
};