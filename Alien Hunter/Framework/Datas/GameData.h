#pragma once

class GData
{
};

//==========��ī��===========//
struct SkyData :public GData
{
	wstring starMap;
	wstring MoonMap;
	wstring glowMap;
	wstring cloudMap;

	float leans;
	float timeFactor;
	int sampleCount;
	D3DXVECTOR3 waveLength;

	float startTheta;

	float cloudScale;
	float cloudSpeed;

	wstring loadFileData;
};
//==========================//

//=========�ͷ���===========//
struct TerrainData :public GData
{
	wstring heightMap;
	float heightScale;
	float heightMapWidth;
	float heightMapHeight;

	wstring layerMap[3];
	D3DXVECTOR4 blendHeight;
	D3DXVECTOR4 activeBlend;

	float DetailValue;
	float DetailIntensity;

	D3DXCOLOR ambient;

	wstring diffuseMap;
	D3DXCOLOR diffuse;

	wstring specularMap;
	D3DXCOLOR specular;
	float intensity;

	wstring normalMap;
	wstring detailMap;

	int texScale;
	float fogStart;
	float fogRange;

	struct BillboardInfo
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotate;
		D3DXVECTOR2 size;
		UINT textureNum;
	};
	vector<BillboardInfo> billboards;

	struct BillboardCollider
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 scale;
	};

	vector<BillboardCollider> billboardCollider;
	wstring loadFileData;
};
//==========================//


//===========����============//
struct RainData :public GData
{
	D3DXVECTOR3 size;
	D3DXVECTOR3 velocity;

	float dist;
	D3DXCOLOR color;
	UINT particleCount;
	wstring loadFileData;
};
//==========================//

//============��==============//
struct ModelData :public GData
{
	wstring modelName;
	wstring type;
	wstring meshFile;
	wstring materialFile;
	vector<class Material*> materials;
	D3DXVECTOR3 min, max;
	vector<class ColliderElement*> colliders;
	vector<class TrailRenderer*> trails;
	wstring loadFileData;
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
};
//=============================//

//============���ϸ��̼�============//
struct AnimData :public GData
{
	ModelData modelData;
	vector<class ModelClip*> clips;
};

//=================================//

//===========�÷��̾�============//
struct PlayerData :public GData
{
	AnimData animData;
	class BehaviorTree*bt;
	vector<class Weapon*> weapons;
};
//==============================//

//==========���ʹ�==============//
struct EnemyData :public GData
{
	AnimData animData;
	class BehaviorTree*bt;
};


