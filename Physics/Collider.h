#pragma once
#include "LineMaker.h"

enum COLLIDERSTATE
{
	NONE = 0, // �ƹ��͵� �������� ��ŷ�� �ǰ� �ؾ���
	BOX,
	CAPSULE
};
struct ColliderInfo // �ݶ��̴��� �������
{
	D3DXVECTOR3 pos;
	float radius;
	int height;
	vector<D3DXVECTOR3> heightPos;
};
class Collider
{
public:
	Collider(
		D3DXMATRIX modelWorld, 
		bool bShowCollider,
		wstring colliderName=L"");
	virtual ~Collider();

	virtual void Render();
	virtual void ImguiRender();

	//�ڽ��� ���� ������ʿ䰡 ����
	virtual void Update(D3DXMATRIX modelWorld);

	virtual void ColliderLineInit() = 0;
	
	//�ݶ��̴��� �ش���� ���� ���ӽ�ų��
	void SetGameModel(class GameModel*model, wstring parentName)
	{
		this->model = model;
		parentObjName = parentName;
	}
	//�ݶ��̴��� 3���������� 
	//���ӿ�����Ʈ���� �ڽ��ʱ�ȭ������������Ѵ�
	void InputColliderInfo
	(
		vector<D3DXVECTOR3>*initBox,
		vector<D3DXVECTOR3>*boundingBox,
		vector<ColliderInfo>*curBox
	);

	//�̸���ȯ
	wstring&GetColliderName() { return name; }

	//�ݶ��̴� ���̴���
	bool&IsShowCollider() { return isShowCollider; }

	//�ݶ��̴� ũ��
	D3DXVECTOR3&ColliderSize() { return colliderSize; }

	//�浹�� �ݶ��̴� ����
	//�ڽ� : 8���� ���� �ִ� �ּ���
	//�� : �߽����� ����������
	//ĸ�� : �߽��� 2���� ����������
	virtual vector<ColliderInfo> InfoCollider() { return *colliderInfo; }

	wstring GetParentObjName() { return parentObjName; }

	COLLIDERSTATE&ColliderState() { return cState; }
protected:
	void UpdateColliderWorld();
protected:
	vector<D3DXVECTOR3>*initPos; // �ִ��ּ��� �ʱ⼳���� �ʿ�
	vector<D3DXVECTOR3>*boundingBox; // ���� �ٿ���ڽ�
	vector<ColliderInfo>*colliderInfo; // ��ġ�ٲ�� ����ؼ����ŵǴ°�				

	//�ݶ��̴� �̸�
	wstring name;

	//�θ������Ʈ �̸�
	wstring parentObjName;

	//�ݶ��̴��� ���̴��� ����
	bool isShowCollider;

	//������
	D3DXCOLOR boundingBoxColor;

	//���� �׸������� Ŭ����
	LineMaker*ColliderLine;

	//�ݶ��̴��� ������ ���� ����
	D3DXMATRIX modelWorld;

	//�ݶ��̴���ü����
	D3DXMATRIX colliderWorld;
	D3DXVECTOR3 cScale, cRotate, cPosition;

	D3DXVECTOR3 colliderSize;

	//�ݶ��̴� ����
	COLLIDERSTATE cState;

	//�ݶ��̴��� x,y,z����
	D3DXVECTOR3 distance;

	//�θ𰡵Ǵ� ��=>�θ���� �θ������Ʈ�� ����
	class GameModel*model;
};
