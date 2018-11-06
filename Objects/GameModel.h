#pragma once
#include "GameRender.h"
#include "../Interfaces/ICollision.h"

class GameModel :public GameRender, public ICollision
{
public:
	GameModel(wstring matFolder,wstring matFile,wstring meshFolder ,wstring meshFile, ExecuteValues*values);
	GameModel(GameModel&gameModel);
	virtual ~GameModel();
	
	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Scale(float x, float y, float z);
	virtual void Scale(D3DXVECTOR3 vec);
	virtual D3DXVECTOR3&Scale();

	virtual void Update();
	virtual void Render();
	virtual void PostRender() {};
	virtual void FixedUpdate() {}; // =>추가 플레이코드
	virtual void FixedInit() {};

	void ColliderSetRender();
	void TrailSetRender();

	//TODO:충돌테스트하기
	virtual bool OnCollisionEnter() { return false; }
	virtual bool OnCollisionExit() { return false; }

	//////////////////////////////////

	//부모모델 받아오는 함수
	//무기클래스에서 사용됨
	virtual void SetParentModel(GameModel*model, wstring parentName) {}
	/////////////////////////////////////////

	//월드행렬 반환함수
	D3DXMATRIX&GetWorldMat() { return World(); }

	//본에서 처리된 본의월드행렬
	D3DXMATRIX GetBoneMatrix(wstring boneName);
	D3DXMATRIX GetBoneMatrix(int idx);

	//모델반환함수
	Model*GetModel() { return model; }

	//쉐이더 설정함수
	void SetShader(Shader*shader);
	
	//디퓨즈 설정함수
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR&color);
	void SetDiffuseMap(wstring file);
	void SetDiffuseMap(class Texture * diffues);
	void SetDiffuseMap(class Texture*diffuse, int idx);

	//스펙큘러 설정함수
	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR&color);
	void SetSpecularMap(wstring file);
	void SetSpecularMap(class Texture * diffues);
	void SetSpecularMap(class Texture * diffues,int idx);

	//노말맵 설정함수
	void SetNormalMap(wstring file);
	void SetNormalMap(class Texture * diffues);
	void SetNormalMap(class Texture * diffues,int idx);

	//디테일맵 설정함수
	void SetDetailMap(wstring file);

	//빛의 강도설정함수
	void SetShininess(float val);
	
	wstring&ModelName() { return modelName; }
	wstring&GetModelType() { return modelType; }
	wstring&ClassName() { return className; }
	virtual GameModel*GetPtr() { return myPtr; }

	D3DXVECTOR3 GetCenterPos();
	D3DXVECTOR3 GetSize();
	bool&PickState() { return isPick; }
	bool&ClickState() { return isOneClick; }

	vector<class Collider*>&GetColliders() { return colliders; }
	vector<class Trail*>&GetTrails() { return trails; }

	vector<D3DXVECTOR3>&InitBoxPos() { return initBoxPos; }
	vector<D3DXVECTOR3>&CurBoxPos() { return curBoxPos; }
	vector<struct ColliderInfo>&MinmaxPos() { return minmaxPos; }
private:
	//////////////피킹을위한 모델의 최대최소점/////////////////
	void PickPosInit();
	void UpdatePickPos();
	//bool IsPicked();

private:
	vector<D3DXVECTOR3> initBoxPos; // 초기 박스 
	vector<D3DXVECTOR3> curBoxPos;  // 갱신되는 박스
	vector<struct ColliderInfo> minmaxPos;  // 박스의 최대최소점

protected:
	void CalcPosition();
	bool IsPicked();
protected:
	GameModel*parentModel;

	GameModel*myPtr; // 외부전달용 임구이에서 객체자체를넘길때
	wstring modelType; // 게임모델인지 에님모델인지 판단
	wstring className; // 클래스이름
	wstring modelName; // 모델이름

	Model*model;
	Shader*shader;
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;
	vector<D3DXMATRIX> initBonesTransforms;
	class ExecuteValues*values;

	bool isPick;
	bool isOneClick;
	bool isColliderOpen; // 컴포넌트랜더쪽에서사용
	bool isTrailOpen;
	
	vector<class Collider*> colliders;
	vector<class Trail*> trails;

private:
	//////////////랜더버퍼/////////////////
	class RenderBuffer :public ShaderBuffer
	{
	public:
		RenderBuffer()
			:ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Index = 0;
		}
		struct Struct
		{
			int Index;

			float Padding[3];
		}Data;
	};
	RenderBuffer*renderBuffer;
	///////////////////////////////////////

	////////////노말visible버퍼////////////
	class NormalBuffer :public ShaderBuffer
	{
	public:
		NormalBuffer()
			:ShaderBuffer(&Data, sizeof(Data))
		{
			Data.isVisible = 0;
		}

		struct Struct
		{
			int isVisible;
			float Padding[3];
		}Data;
	};
	///////////////////////////////////////

public:
	//외부설정값으로 사용할값
	D3DXCOLOR diffuseColor = { 1,1,1,1 };
	D3DXCOLOR specularColor = { 1,1,1,1 };
	float shiness = 1;
	bool isVisible;
	bool isActive;

	NormalBuffer*normalBuffer;
};