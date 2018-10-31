#pragma once
#include "GameTool.h"

class CameraTool :public GameTool
{
public:
	
public:
	CameraTool(class ExecuteValues*values);
	~CameraTool();
	void Update() override;
	void Render() override;
	void HirakyRender() override;
	void AssetRender() override;
	void InspectRender() override;

	virtual void InputCameraPtr(class Frustum*frustum)
	{
		this->frustum = frustum;
	}
	
private:
	
private:
	class ExecuteValues*values;
	class Frustum*frustum;
};

