#pragma once

//������ִ� ���� �ҷ��ͼ�
//������Ұ����� ���ϱ�
//�Ѿ��� unorderedmap���� �����ϱ�
//���̸�,���Ӹ�*

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
		bool isThrough; // ����
	};
private:
	typedef unordered_map<wstring, vector<BulletDesc>>::iterator bulletIter;
private:

	// �Ѿ��� 30�������� ��������
	unordered_map<wstring, vector<BulletDesc>> bulletMap;

};
