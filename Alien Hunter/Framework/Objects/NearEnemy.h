#pragma once

class NearEnemy :public Enemy
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
public:
	NearEnemy(Effect*lineEffect = NULL);
	NearEnemy(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect = NULL);
	NearEnemy(NearEnemy&copy);

	virtual ~NearEnemy();

	bool EnemyState(wstring teskName) override;

	virtual class GData* Save();
	virtual void Load(wstring fileName);

	virtual void ColliderUpdate();

	virtual bool&LoadStart();

private:
	void UpdateWorld() override;

	void Turn(float angle);

private:
	typedef map<wstring, float>::iterator mIter;

private:
	EState eState;
	map<wstring, float> timeMap;
	map<wstring, float> resTimeMap;

	float turnValue;
	bool isDamaged = false;
	bool isOnceAtk = false;
	vector<D3DXVECTOR3> patrolDirs;
};
