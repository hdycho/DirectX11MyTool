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

	//���ɸ��Ͱ� ������ ������ ����(����ǥ��)
	BonesMap bonesMap;
	BonesScaleMap bonesScaleMap;

	vector<class ModelClip *>*animVec;	//  ���ϸ��̼����ϵ�
	
	wstring modelType;
	class GameAnimModel* aModel;
	class GameModel* nModel;
	//���¿��� ���̶�Ű�� ���� �̸�������//
	wstring dragAnimName;
	D3DXVECTOR3 dragAnimColor;
	////////////////////////////////////

	//�ӱ��� �޺��� ���� ������Ʈ�ϴ°���//
	map<wstring,vector<wstring>> animNames; // �̰� ���ڴٸ����־����
	wstring selectAnimName;	   // ���ڴٸ����־����
	vector<wstring> modelNames;   
	wstring selectmodelName;

	vector<wstring> selectColliders;
	wstring colliderName;

	vector<wstring> selectTrails;
	wstring trailName;
	///////////////////////////////////////

	CHARACTOR_SETTYPE sType;

	class ExecuteValues*values;

	//���ϸ��̼� ������� ����� ����
	bool isRepeat;
	float blendTime;
	float startTime;
	float speed;
};