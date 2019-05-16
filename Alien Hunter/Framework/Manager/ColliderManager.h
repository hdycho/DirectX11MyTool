#pragma once
#include "Interface\ICollider.h"

class ColliderManager
{
	//모든콜라이더를 상속받는 객체의에 콜라이더
	//업데이트를 대신돌려주고
	//레이어에 콜라이더추가할수 있게만들기

public:
	static void Create();
	static void Delete();
	static ColliderManager*Get() { return instance; }

private:
	static ColliderManager*instance;

private:
	typedef vector<ICollider*>::iterator vIter;

public:
	ColliderManager();
	~ColliderManager();

	//===========콜라이더레이어=============//
	void CreateLayer(wstring layerName);
	void AddLayerToCollider(ColliderElement*collider);
	void AddLayerToCollider(wstring layerName, ColliderElement*collider);
	void ImGuiLayerMenu();
	//=====================================//

	//===========콜라이더자체==============//
	void InputObject(ICollider*obj);
	void Erase(ICollider*obj);
	void Update(); // 콜라이더위치갱신
				   //===================================//

	void DeleteAll();

private:
	vector<ICollider*> colliderVec;
	vector<wstring> layerNames;
	wstring selectLayerName;

	wstring inputColliderName;

};
