#pragma once

class GameTool
{
public:
	GameTool(wstring toolName):toolName(toolName)
	{}
	virtual void Render() = 0;
	virtual void HirakyRender()=0;
	virtual void AssetRender() = 0;
	virtual void InspectRender() = 0;
	virtual void Update()=0;

	//�ɸ�����
	virtual void InputModelPtr(class GameModel*model) {}
	virtual void SetAnimVec(vector<class ModelClip *>&animVec) {}
	virtual class GameModel*GetModelPointer() { return NULL; }
	virtual void SetType(int num) {};

	//����
	virtual void InputTerrainPtr(class TerrainRender*terrain) {}
	virtual class TerrainRender*GetTerrainPointer() { return NULL; }

	//ī�޶���
	virtual void InputCameraPtr(class Frustum*frustum) {}
	//virtual class QuardTree*GetTerrainPointer() { return NULL; }
protected:
	wstring toolName; // ���̸�
};