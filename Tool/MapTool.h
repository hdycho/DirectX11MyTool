#pragma once
#include "GameTool.h"

class MapTool :public GameTool
{
public:
	enum MAP_SETTYPE
	{
		SET_NONE,
		SET_DETAIL_TERRAIN,
		SET_BRUSH_TERRAIN
	};
public:
	MapTool(class ExecuteValues*values);
	~MapTool();
	void Update() override;
	void Render() override;
	void HirakyRender() override;
	void AssetRender() override;
	void InspectRender() override;
	
	void InputTerrainPtr(class TerrainRender*terrain) override;
	class TerrainRender*GetTerrainPointer() override;

	virtual void SetType(int num) { mSetType = (MAP_SETTYPE)num; }
private:
	void LoadTerrainMapFile(class TerrainRender*tree, wstring filename, int type);
	void BasicSetRender();
private:
	class ExecuteValues*values;
	class TerrainRender*terrain;

	MAP_SETTYPE mSetType;


};
