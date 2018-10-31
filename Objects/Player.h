#pragma once
#include "GameAnimModel.h"
#include "../Physics/Trail.h"

class Player :public GameAnimModel
{
public:
	enum class PLAYERSTATE
	{
		NONE=-1,
		IDLE,
		MOVE,
		ATTACK
	};
public:
	Player(wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile,
		ExecuteValues*values);
	
	Player(Player&player);

	virtual ~Player();
	virtual bool OnCollisionEnter();
	virtual bool OnCollisionExit();

	void Update() override;
	void FixedUpdate() override;
	void Render() override;
	void PostRender() override;
private:
	bool Move(); // 그냥 방향키방향에따라움직임
	bool MoveDirect(); // 앞에보는방향으로만 이동
	

private:
	PLAYERSTATE pState;
};