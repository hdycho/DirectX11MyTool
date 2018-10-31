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
	bool Move(); // �׳� ����Ű���⿡���������
	bool MoveDirect(); // �տ����¹������θ� �̵�
	

private:
	PLAYERSTATE pState;
};