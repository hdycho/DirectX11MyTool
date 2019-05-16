#pragma once

class Weapon :public GameModel
{
public:
	Weapon(UINT instanceId,
		wstring matFile,
		wstring meshFile,
		Player*player,
		Effect*lineEffect = NULL);

	Weapon(Weapon&copy, Player*player);

	virtual ~Weapon();

	virtual void Ready();
	virtual void Update();
	virtual void Render();

	void TargetWorld(int boneIdx);
	int TargetWorldBoneIndex() { return boneIndex; }
	wstring MaterialFileName() { return matFile; }
	wstring MeshFileName() { return meshFile; }

public:
	int loadBoneIndex = -2;
	D3DXVECTOR3 loadScale, loadRotate, loadPos;

protected:
	virtual void UpdateWorld();

private:
	Player*player;
	int boneIndex;

	wstring matFile;
	wstring meshFile;
};
