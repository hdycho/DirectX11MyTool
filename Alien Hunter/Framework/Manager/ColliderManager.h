#pragma once
#include "Interface\ICollider.h"

class ColliderManager
{
	//����ݶ��̴��� ��ӹ޴� ��ü�ǿ� �ݶ��̴�
	//������Ʈ�� ��ŵ����ְ�
	//���̾ �ݶ��̴��߰��Ҽ� �ְԸ����

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

	//===========�ݶ��̴����̾�=============//
	void CreateLayer(wstring layerName);
	void AddLayerToCollider(ColliderElement*collider);
	void AddLayerToCollider(wstring layerName, ColliderElement*collider);
	void ImGuiLayerMenu();
	//=====================================//

	//===========�ݶ��̴���ü==============//
	void InputObject(ICollider*obj);
	void Erase(ICollider*obj);
	void Update(); // �ݶ��̴���ġ����
				   //===================================//

	void DeleteAll();

private:
	vector<ICollider*> colliderVec;
	vector<wstring> layerNames;
	wstring selectLayerName;

	wstring inputColliderName;

};
