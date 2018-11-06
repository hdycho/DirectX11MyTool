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
	virtual void FixedUpdate() {}; // =>�߰� �÷����ڵ�
	virtual void FixedInit() {};

	void ColliderSetRender();
	void TrailSetRender();

	//TODO:�浹�׽�Ʈ�ϱ�
	virtual bool OnCollisionEnter() { return false; }
	virtual bool OnCollisionExit() { return false; }

	//////////////////////////////////

	//�θ�� �޾ƿ��� �Լ�
	//����Ŭ�������� ����
	virtual void SetParentModel(GameModel*model, wstring parentName) {}
	/////////////////////////////////////////

	//������� ��ȯ�Լ�
	D3DXMATRIX&GetWorldMat() { return World(); }

	//������ ó���� ���ǿ������
	D3DXMATRIX GetBoneMatrix(wstring boneName);
	D3DXMATRIX GetBoneMatrix(int idx);

	//�𵨹�ȯ�Լ�
	Model*GetModel() { return model; }

	//���̴� �����Լ�
	void SetShader(Shader*shader);
	
	//��ǻ�� �����Լ�
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR&color);
	void SetDiffuseMap(wstring file);
	void SetDiffuseMap(class Texture * diffues);
	void SetDiffuseMap(class Texture*diffuse, int idx);

	//����ŧ�� �����Լ�
	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR&color);
	void SetSpecularMap(wstring file);
	void SetSpecularMap(class Texture * diffues);
	void SetSpecularMap(class Texture * diffues,int idx);

	//�븻�� �����Լ�
	void SetNormalMap(wstring file);
	void SetNormalMap(class Texture * diffues);
	void SetNormalMap(class Texture * diffues,int idx);

	//�����ϸ� �����Լ�
	void SetDetailMap(wstring file);

	//���� ���������Լ�
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
	//////////////��ŷ������ ���� �ִ��ּ���/////////////////
	void PickPosInit();
	void UpdatePickPos();
	//bool IsPicked();

private:
	vector<D3DXVECTOR3> initBoxPos; // �ʱ� �ڽ� 
	vector<D3DXVECTOR3> curBoxPos;  // ���ŵǴ� �ڽ�
	vector<struct ColliderInfo> minmaxPos;  // �ڽ��� �ִ��ּ���

protected:
	void CalcPosition();
	bool IsPicked();
protected:
	GameModel*parentModel;

	GameModel*myPtr; // �ܺ����޿� �ӱ��̿��� ��ü��ü���ѱ涧
	wstring modelType; // ���Ӹ����� ���Ը����� �Ǵ�
	wstring className; // Ŭ�����̸�
	wstring modelName; // ���̸�

	Model*model;
	Shader*shader;
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;
	vector<D3DXMATRIX> initBonesTransforms;
	class ExecuteValues*values;

	bool isPick;
	bool isOneClick;
	bool isColliderOpen; // ������Ʈ�����ʿ������
	bool isTrailOpen;
	
	vector<class Collider*> colliders;
	vector<class Trail*> trails;

private:
	//////////////��������/////////////////
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

	////////////�븻visible����////////////
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
	//�ܺμ��������� ����Ұ�
	D3DXCOLOR diffuseColor = { 1,1,1,1 };
	D3DXCOLOR specularColor = { 1,1,1,1 };
	float shiness = 1;
	bool isVisible;
	bool isActive;

	NormalBuffer*normalBuffer;
};