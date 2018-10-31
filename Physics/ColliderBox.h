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
	vector<D3DXVECTOR3> setBox; // �ڽ�ũ�������������پ��°�
	vector<D3DXVECTOR3> resultBox; // �ڽ�ũ��*������
	vector<D3DXVECTOR3> minMaxPos; // �پ���ٽ� �ִ��ּҴٽñ���
	vector<ColliderInfo> boxInfo; // �ִ��ּұ��Ѱ� �ڽ��� �ֱ�

	
};
