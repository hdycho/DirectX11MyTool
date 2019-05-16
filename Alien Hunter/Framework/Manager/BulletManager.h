#pragma once

//저장되있는 모델을 불러와서
//몇개생성할것인지 정하기
//총알은 unorderedmap으로 관리하기
//모델이름,게임모델*

class BulletManager
{
public:
	static BulletManager* Get();
	static void Delete();

private:
	static BulletManager*instance;

public:
	BulletManager();
	~BulletManager();

	void BulletInit(wstring bulletName, wstring dataFile, UINT bulletNum, float speed, float range, bool isGravity = false);
	void BulletUpdate();
	void BulletRender();

	void ReloadBullet(wstring bulletName,D3DXVECTOR3 startPos);

	void Shot(wstring bulletName, D3DXVECTOR3 dir, D3DXVECTOR3 startPos,D3DXVECTOR3 rotate,float speed=10,float size=3,bool isThrough=false);

	void BulletColliderInit(wstring bulletName);
	
	void RemoveAll();

private:
	struct BulletDesc
	{
		GameModel*bullet;
		float speed;
		bool isGravity;
		float range;
		D3DXVECTOR3 dir;
		D3DXVECTOR3 startPos;
		D3DXVECTOR3 curPos;
		bool isShot;
		bool isThrough; // 관통
	};
private:
	typedef unordered_map<wstring, vector<BulletDesc>>::iterator bulletIter;
private:

	// 총알은 30발정도로 제한하자
	unordered_map<wstring, vector<BulletDesc>> bulletMap;

};
