#pragma once
#include "GameTool.h"

class CharactorTool :public GameTool
{
public:
	enum CHARACTOR_SETTYPE
	{
		SET_NONE,
		SET_MESH,
		SET_ANIMATION
	};
private:
	typedef map<wstring, vector<class BoneSphere*>> BonesMap;
	typedef map<wstring, vector<class BoneSphere*>>::iterator bonesIter;

	typedef map<wstring, vector<D3DXVECTOR3>> BonesScaleMap;
	typedef map<wstring, vector<D3DXVECTOR3>>::iterator bonesScaleIter;
public:
	CharactorTool(class ExecuteValues*values);
	~CharactorTool();
	void Update() override;
	void Render() override;
	void HirakyRender() override;
	void AssetRender() override;
	void InspectRender() override;
	void InputModelPtr(class GameModel*model) override;
	virtual class GameModel*GetModelPointer() 
	{ 
		if (modelType == L"DefaultAnim")
			return (GameModel*)aModel;
		else
			return nModel;
	}

	virtual void SetAnimVec(vector<class ModelClip *>&animVec);

	virtual void SetType(int num) { sType = (CHARACTOR_SETTYPE)num; }

	void DrawBoneNameText();

	void InputAnimNames(wstring modelName, wstring animName);
private:

	void InputBoneData(class Shader*shader,class GameModel*model,wstring modelName,int idx);

	void UpdateBoneData();

	int GetClipIndex();

	void LoadShaderFile(wstring filename);

private:
	wstring shaderName;

	//한케릭터가 가지는 뼈들의 정보(구로표현)
	BonesMap bonesMap;
	BonesScaleMap bonesScaleMap;

	vector<class ModelClip *>*animVec;	//  에니메이션파일들
	
	wstring modelType;
	class GameAnimModel* aModel;
	class GameModel* nModel;
	//에셋에서 하이라키에 보낼 이름정보들//
	wstring dragAnimName;
	D3DXVECTOR3 dragAnimColor;
	////////////////////////////////////

	//임구이 콤보에 들어가서 업데이트하는값들//
	map<wstring,vector<wstring>> animNames; // 이거 각자다르게있어야함
	wstring selectAnimName;	   // 각자다르게있어야함
	vector<wstring> modelNames;   
	wstring selectmodelName;

	vector<wstring> selectColliders;
	wstring colliderName;

	vector<wstring> selectTrails;
	wstring trailName;
	///////////////////////////////////////

	CHARACTOR_SETTYPE sType;

	class ExecuteValues*values;

	//에니메이션 재생에서 사용할 값들
	bool isRepeat;
	float blendTime;
	float startTime;
	float speed;
};