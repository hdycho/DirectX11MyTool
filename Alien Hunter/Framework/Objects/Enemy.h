#pragma once

class Enemy :public GameAnimator
{
public:
	Enemy(Effect*lineEffect = NULL);
	Enemy(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect = NULL);
	Enemy(Enemy&copy);
	virtual ~Enemy();

	void Ready() override;
	void Update() override;
	void Render() override;

	void GetTerrain(class Terrain*terrain)
	{
		this->terrain = terrain;
	}

	void GetPlayer(class Player*player)
	{
		this->player = player;
	}

	virtual bool EnemyState(wstring teskName) = 0;
	BehaviorTree*GetBT() { return BT; }

	virtual class GData* Save();
	virtual void Load(wstring fileName);

	virtual void ColliderUpdate();

	virtual bool&LoadStart();
	
	int&Hp() { return hp; }
	int&InitHp() { return initHp; }

protected:
	virtual void UpdateWorld() {};

private:
	class Terrain*terrain;

protected:
	class Player*player;
	BehaviorTree*BT;
	vector<wstring> behaviors;

	float walkSpeed;
	int hp;
	int initHp;

	bool isDie = false;
};