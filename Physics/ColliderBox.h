#pragma once
#include "Collider.h"

class ColliderBox :public Collider
{
public:
	ColliderBox(
		D3DXMATRIX modelMat,
		COLLIDERSTATE cState,
		bool isShowCollider=false,
		wstring name = L"");
	~ColliderBox();

	void Update(D3DXMATRIX modelWorld) override;
	void Render() override;
	void ImguiRender() override;

	void RenewBox(D3DXMATRIX modelWorld);
	void ColliderLineInit() override;

	virtual vector<ColliderInfo> InfoCollider() { return boxInfo; }
private:
	vector<D3DXVECTOR3> setBox; // 박스크기조정했을때줄어드는값
	vector<D3DXVECTOR3> resultBox; // 박스크기*월드결과
	vector<D3DXVECTOR3> minMaxPos; // 줄어들면다시 최대최소다시구함
	vector<ColliderInfo> boxInfo; // 최대최소구한거 박스에 넣기

	
};
