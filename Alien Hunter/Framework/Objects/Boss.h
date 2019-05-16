#pragma once

class Boss :public Enemy
{
public:
	enum class EState
	{
		IDLE,
		PATROL,
		ATTACK,
		MOVE,
		DAMAGED,
		DIE,
	};

	enum BossState
	{
		LEVEL1,
		LEVEL2,
		LEVEL3
	};
public:
	Boss(Effect*lineEffect = NULL);
	Boss(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect = NULL);
	Boss(Boss&copy);

	virtual ~Boss();

	bool EnemyState(wstring teskName) override;

	bool IsAttack2() { return isAttack2; }
	bool IsAttack3() { return isAttack3; }

	virtual class GData* Save();
	virtual void Load(wstring fileName);

	virtual void ColliderUpdate();

	virtual bool&LoadStart();

private:
	void UpdateWorld() override;

	void Turn(float angle);

	void InitAtks();
	void HpUpdate();
	void AttackUpdate();
private:
	typedef map<wstring, float>::iterator mIter;

private:
	EState eState;
	BossState bState;
	map<wstring, float> timeMap;
	map<wstring, float> resTimeMap;

	float turnValue;
	bool isDamaged = false;
	bool isOnceAtk = false;
	bool isOnceBomb = false;

	bool isAttack2 = false;
	bool isAttack3 = false;

	vector<D3DXVECTOR3> patrolDirs;

	int atkIdx;
	int atk1[5];
	int atk2[5];
	int atk3[5];
};
