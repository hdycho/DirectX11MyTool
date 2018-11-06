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
		AIM,
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

	void SetCamera(class TPSCamera*cam) { this->cam = cam; }
	void SetTerrain(class TerrainRender*terrain) { this->terrain = terrain; }
	D3DXVECTOR3 GetShotStartPos() {return shotStart;}
	D3DXVECTOR3 GetShotDir() { return shotDir; }
	bool&IsShot() { return isShot; }
private:
	bool Move(); // 그냥 방향키방향에따라움직임
	bool MoveDirect(); // 앞에보는방향으로만 이동
	bool MoveTPS();
	void Turn();

	bool IsMoveOnce();
	bool IsMoveStay();
private:
	class TPSCamera*cam;
	PLAYERSTATE pState;

	Texture*target;
	Render2D*render;
	BlendState* blendMode[2];

	float saveTargetY;
	D3DXVECTOR3 test;
	class LineMaker*shotLine;
	class TerrainRender*terrain;

	D3DXVECTOR3 shotStart;
	D3DXVECTOR3 shotDir;
	bool isShot;
};