#pragma once
#include "stdafx.h"

class Bullet
{
private:
	struct bulletModel
	{
		class GameModel*model;
		class ColliderBox*collider;
		bool isShot;
		float gravity;
		float addGravity;
	};
private:
	static Bullet*instance;
	typedef vector<bulletModel*> bulletVec;
	typedef map<wstring, bulletVec> bulletMap;
public:
	static Bullet*GetInstance();

private:
	bulletMap bMap;
	map<wstring, int> bulletIdxMap;
public:
	void BulletSetting(wstring matFolder,
		wstring matFile,
		wstring meshFolder,
		wstring meshFile, int bNum, wstring bulletName);

	void Update();
	void Render();

	void Release();

	void Shot(wstring bulletName, D3DXVECTOR3 shotPos, D3DXVECTOR3 shotVector, float speed,float gravity=0.0f);
	void RenewColliderBox(wstring bulletName, int i);
	vector<bulletModel*> GetBullet(wstring bulletName)
	{
		return bMap.find(bulletName)->second;
	}

	int GetBulletSize(wstring bulletName) { return bMap.find(bulletName)->second.size(); }
};

