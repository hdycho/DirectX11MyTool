#pragma once
#include "LineMaker.h"

enum COLLIDERSTATE
{
	NONE = 0, // 아무것도 없을때도 픽킹은 되게 해야함
	BOX,
	CAPSULE
};
struct ColliderInfo // 콜라이더의 모든정보
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

	//박스나 본은 사용할필요가 없음
	virtual void Update(D3DXMATRIX modelWorld);

	virtual void ColliderLineInit() = 0;
	
	//콜라이더가 해당모델의 뼈에 종속시킬때
	void SetGameModel(class GameModel*model, wstring parentName)
	{
		this->model = model;
		parentObjName = parentName;
	}
	//콜라이더의 3가지정보들어감 
	//게임오브젝트에서 박스초기화끝나고보내줘야한다
	void InputColliderInfo
	(
		vector<D3DXVECTOR3>*initBox,
		vector<D3DXVECTOR3>*boundingBox,
		vector<ColliderInfo>*curBox
	);

	//이름반환
	wstring&GetColliderName() { return name; }

	//콜라이더 보이는지
	bool&IsShowCollider() { return isShowCollider; }

	//콜라이더 크기
	D3DXVECTOR3&ColliderSize() { return colliderSize; }

	//충돌시 콜라이더 정보
	//박스 : 8개의 점중 최대 최소점
	//구 : 중심점과 반지름정보
	//캡슐 : 중심점 2개와 반지름정보
	virtual vector<ColliderInfo> InfoCollider() { return *colliderInfo; }

	wstring GetParentObjName() { return parentObjName; }

	COLLIDERSTATE&ColliderState() { return cState; }
protected:
	void UpdateColliderWorld();
protected:
	vector<D3DXVECTOR3>*initPos; // 최대최소점 초기설정시 필요
	vector<D3DXVECTOR3>*boundingBox; // 실제 바운딩박스
	vector<ColliderInfo>*colliderInfo; // 위치바뀌면 계속해서갱신되는값				

	//콜라이더 이름
	wstring name;

	//부모오브젝트 이름
	wstring parentObjName;

	//콜라이더선 보이는지 유무
	bool isShowCollider;

	//선색깔
	D3DXCOLOR boundingBoxColor;

	//선을 그리기위한 클래스
	LineMaker*ColliderLine;

	//콜라이더를 가지는 모델의 월드
	D3DXMATRIX modelWorld;

	//콜라이더자체월드
	D3DXMATRIX colliderWorld;
	D3DXVECTOR3 cScale, cRotate, cPosition;

	D3DXVECTOR3 colliderSize;

	//콜라이더 상태
	COLLIDERSTATE cState;

	//콜라이더의 x,y,z길이
	D3DXVECTOR3 distance;

	//부모가되는 모델=>부모뼈를 부모오브젝트로 설정
	class GameModel*model;
};
