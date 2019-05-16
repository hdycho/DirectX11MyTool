#pragma once

class Player :public GameAnimator
{
public:
	enum WeaponState
	{
		RAYCASTGUN = 2,
		MUCHINGUN,
		FIREGUN
	}weaponState;

public:
	Player(Effect*lineEffect = NULL);
	Player(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect = NULL);
	Player(Player&copy);
	virtual ~Player();

	void Ready() override;
	void Update() override;
	void Render() override;

	void UiUpdate();

	void GetTerrain(class Terrain*terrain)
	{
		this->terrain = terrain;
	}

	void GetBoss(class Boss*boss)
	{
		this->boss = boss;
	}

	bool PlayerState(wstring teskName);
	BehaviorTree*GetBT() { return BT; }

	vector<class Weapon*>&GetWeapons();

	virtual class GData* Save();
	virtual void Load(wstring fileName);

	virtual void ColliderUpdate();

	D3DXVECTOR3&GetCamRotation() { return camRotate; }
	D3DXVECTOR3&GetCamPosition();

	virtual bool&LoadStart();
	bool IsFireGun() { return isFireGun; }
	bool&IsUpdate() { return isUpdate; }
	bool IsDie() { return isDie; }
	bool IsZoom() { return isZoom; }

	void SetCamera(class TPSCamera*camera) { this->cam = camera; }
private:
	bool MoveTPS();
	void MoveShotting();
	void Turn();

	void UpdateWorld() override;
private:
	class Boss*boss;

	class TPSCamera*cam;

	class Terrain*terrain;
	D3DXVECTOR3 camRotate; //xȸ�� ���ϰ� ����ó���Ϸ���
	D3DXVECTOR3 camPos;	// inplace���ϸ��̼ǾƴѰ͵�ó��

						//�÷��̾����
						//Idle,Move,Attack
	BehaviorTree*BT;
	vector<wstring> behaviors;

	//�÷��̾�����
	//hp,mp,speed
	int hp = 8;
	int bullet1 = 2;
	int bullet2 = 10;

	float walkSpeed;
	float runSpeed;
	bool isOneShot = false;
	bool isDamaged = false;
	bool isDie = false;
	bool isFireGun = false;
	bool isUpdate = true;
	bool isZoom = false;

	bool leftDash = false;
	bool rightDash = false;
	bool backDash = false;
	float gValue = 1;
	float hitValue = 20;

	//�÷��̾��
	vector<class Weapon*> weapons;

	D3DXVECTOR3 crossVec;
	float rebound;

	D3DXVECTOR3 shotPos;
};