#pragma once
#include "Collider.h"

class ColliderCapsule :public Collider
{
public:
	ColliderCapsule(
		D3DXMATRIX modelMat,
		COLLIDERSTATE cState,
		bool isShowCollider = false,
		wstring name = L"");
	~ColliderCapsule();

	void Update(D3DXMATRIX modelWorld) override;
	void Render() override;
	void ImguiRender() override;

	void ColliderLineInit() override;
	void RenewCapsule(D3DXMATRIX modelWorld);
	void DrawCircle(vector<D3DXVECTOR3>&circlePos,int count,int startIdx, int endIdx,int type);

	virtual vector<ColliderInfo> InfoCollider() { return capsuleInfo; }
private:
	D3DXMATRIX curMat, initWorld;
	D3DXVECTOR3 resTrans, resRotate, resScale;
	vector<D3DXVECTOR3> verticalLines;

	vector<ColliderInfo> capsuleInfo;
	ColliderInfo Pos;
	int CircleCount;
	int totalCount;
};

