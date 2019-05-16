#pragma once
#include "Datas\GameData.h"

class FileManager
{
public:
	enum class FileType
	{
		Sky,
		Terrain,
		Rain,
		Model,
		Animation,
		Player,
		Enemy1,
		Boss
	};
public:
	static void Create();
	static void Delete();

	static FileManager* Get() { return instance; }
private:
	static FileManager*instance;

public:
	void DataPush(FileType type, class IData*data);
	void Erase(IData*obj);

public:
	//단일데이터형 저장
	void Save(wstring fileName);
	void SaveScene(wstring fileName,UINT dataSize);
	void Load(wstring sceneName,bool modelsCopy,Effect*lineEffect=NULL);
	void SaveWithFolder(wstring folderName);
	map<wstring, UINT> dataIndexMap;
private:
	typedef map<FileType, vector<class IData*>>::iterator DataIter;
	typedef map<wstring, UINT>::iterator DataIdxIter;
private:
	map<FileType, vector<class IData*>> dataMap;

	UINT terrainIdx = 0;
	UINT rainIdx = 0;
	UINT skyIdx = 0;
};

class ObjectManager
{
public:
	static void Create();
	static void Delete();

	static ObjectManager* Get() { return instance; }
private:
	static ObjectManager*instance;

public:
	ObjectManager();
	~ObjectManager();

	void AddModel(wstring modelName, class GameModel*model);
	void AddTerrain(class Terrain*terrain);
	void AddRain(class Rain*rain);
	void AddSky(class Sky*sky);

	void Update();
	void Ready(); // 여기서 콜라이더 추가및 기본셋팅
	void PreRender();
	void Render();
	void PostRender();

	class GameModel*GetModel(wstring name,int idx);
	class vector<GameModel*>&GetModelVector(wstring name);
	class GameModel*GetPlayer();
	class Shadow*GetShadow() { return shadow; }

private:
	typedef map<wstring, vector<class GameModel*>>::iterator mIter;

private:
	class Terrain*terrain=NULL;
	class Rain*rain=NULL;
	class Sky*sky=NULL;
	class Shadow*shadow=NULL;

	map<wstring, vector<class GameModel*>> models;

	class Frustum*frustum;
};
