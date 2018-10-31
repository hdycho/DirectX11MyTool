#include "Collider.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/Model.h"
#include "./Objects/GameModel.h"

Collider::Collider(D3DXMATRIX modelWorld, bool bShowCollider, wstring colliderName)
	:modelWorld(modelWorld), 
	isShowCollider(bShowCollider),
	name(colliderName)
{
	model = NULL;
	boundingBoxColor = { 1,0,0,1 };

	colliderSize = { 1,1,1 };
	D3DXMatrixIdentity(&colliderWorld);
	cScale = { 1,1,1 };
	cRotate = { 0,0,0 };
	cPosition = { 0,0,0 };

	parentObjName = L"None";
}

Collider::~Collider()
{
}

void Collider::Render()
{
}

void Collider::ImguiRender()
{
}

void Collider::Update(D3DXMATRIX modelWorld)
{
}

void Collider::InputColliderInfo(vector<D3DXVECTOR3>* initBox, vector<D3DXVECTOR3>* boundingBox, vector<ColliderInfo>*curBox)
{
	this->initPos = initBox;
	this->boundingBox = boundingBox;
	this->colliderInfo = curBox;
}

void Collider::UpdateColliderWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, cScale.x, cScale.y, cScale.z);
	//   짐벌락 해결 방법 
	//   1. YawPitchRoll 함번에 처리
	//   2. 사원수 사용(Quaterion) :  
	//   TODO : Quaterion 찾아보기
	D3DXMatrixRotationYawPitchRoll(&R, cRotate.y, cRotate.x, cRotate.z);
	D3DXMatrixTranslation(&T, cPosition.x, cPosition.y, cPosition.z);

	colliderWorld = S * R * T;
}



