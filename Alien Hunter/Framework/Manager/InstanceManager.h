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

	//Type������ �ν��Ͻ��� �־��ִ� �Լ�
	void AddModel(class GameModel*model,wstring shaderFile, InstanceType instType);
	void AddModel(class GameModel*model, Effect*effect, InstanceType instType);

	void DeleteModel(class GameModel*model,UINT instanceID, InstanceType instType);

	//���Ӹ𵨹� ���ϸ����Ϳ� UpdateWorld�� �־��ָ��
	void UpdateWorld(class GameModel*model, UINT instNum, D3DXMATRIX&world, InstanceType instType);
	
	class ModelInstance*FindModel(wstring name);
	class AnimInstance*FindAnim(wstring name);

	int GetIsRenderModel(wstring name);
	int GetIsRenderAnim(wstring name);

	void Render(UINT tec=0);
	void Update();

	void RemoveAllModel();
};
