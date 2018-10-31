#pragma once

class CollisionManager
{
	//충돌은 벡터로관리하고
	//충돌처리는 0번째가 자기본체의 콜라이더이고
	//그이후 1번이후로 충돌처리에 관여함

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

