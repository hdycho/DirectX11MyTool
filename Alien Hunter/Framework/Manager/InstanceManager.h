#pragma once

class InstanceManager
{
public:
	enum class InstanceType
	{
		MODEL,
		ANIMATION
	};
public:
	static void Create();
	static void Delete();
	static InstanceManager*Get() { return instance; }

private:
	static InstanceManager*instance;

private:
	typedef unordered_map<wstring,class ModelInstance*>::iterator modelsIter;
	typedef unordered_map<wstring, class AnimInstance*>::iterator animsIter;

public:
	InstanceManager();
	~InstanceManager();

	unordered_map<wstring,class ModelInstance*> modelInstance;
	unordered_map<wstring, class AnimInstance*> animInstance;

	//Type에따라 인스턴스모델 넣어주는 함수
	void AddModel(class GameModel*model,wstring shaderFile, InstanceType instType);
	void AddModel(class GameModel*model, Effect*effect, InstanceType instType);

	void DeleteModel(class GameModel*model,UINT instanceID, InstanceType instType);

	//게임모델및 에니메이터에 UpdateWorld에 넣어주면됨
	void UpdateWorld(class GameModel*model, UINT instNum, D3DXMATRIX&world, InstanceType instType);
	
	class ModelInstance*FindModel(wstring name);
	class AnimInstance*FindAnim(wstring name);

	int GetIsRenderModel(wstring name);
	int GetIsRenderAnim(wstring name);

	void Render(UINT tec=0);
	void Update();

	void RemoveAllModel();
};
