#pragma once

class CollisionManager
{
	//�浹�� ���ͷΰ����ϰ�
	//�浹ó���� 0��°�� �ڱ⺻ü�� �ݶ��̴��̰�
	//������ 1�����ķ� �浹ó���� ������

private:
	typedef vector<class GameModel*> Models;

	typedef map<wstring, vector<wstring>>::iterator ster;
private:
	static CollisionManager*instance;
	Models model;
	vector<wstring> modelNames;

	int sizeNum = 0;
	int saveNum = -1;
private:
	bool IsCollision(enum COLLIDERSTATE cstate,vector<struct ColliderInfo>& model1, vector<struct ColliderInfo>& model2);
public:
	static CollisionManager*GetInstance();

	void Update();
	void Render();

};

