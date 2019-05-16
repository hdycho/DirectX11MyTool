#pragma once

class GameModel : public GameRender
{
public:
	GameModel(Effect*lineEffect=NULL);
	GameModel(
		UINT instanceId,
		wstring matFile,
		wstring meshFile,
		Effect*lineEffect = NULL);

	GameModel(GameModel&copy);
	void DeleteModel();

	virtual ~GameModel();

	void ResetModelData(UINT instID, wstring matFile, wstring meshFile);

	virtual void Ready();
	virtual void Update();
	virtual void Render();

	Model * &GetModel() { return model; }

	virtual class GData* Save();
	virtual void Load(wstring fileName);

#pragma region Material
	void SetShader(wstring shaderFile);
	void SetShader(Effect*effect);

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);


	virtual void SetShader(D3DXMATRIX v, D3DXMATRIX p);
	virtual void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv);
#pragma endregion

	virtual bool IsPicked();
	virtual void SetPickState(bool val);

	virtual vector<ColliderElement*>&GetColliders();
	virtual void CreateCollider(int type);
	virtual void ColliderUpdate();

	void PickUpdate();

	vector<TrailRenderer*>&GetTrails() { return trails; }

	virtual D3DXMATRIX GetPlayerBoneWorld(int idx);
protected:
	virtual void UpdateWorld();

protected:
	Model* model;
	vector<ColliderElement*> colliders;
	vector<TrailRenderer*> trails;

	UINT instanceID;

public:
	virtual D3DXMATRIX GetBoneMatrix(int idx);
	UINT&InstanceID() { return instanceID; }

protected:
	Effect*lineEffect;

protected:
	wstring matFile;
	wstring meshFile;
public:
	wstring MaterialFile() { return matFile; }
	wstring MeshFile() { return meshFile; }
protected:
	UINT pass;

	wstring typeName;

	vector<class Material*> loadMaterial;
	bool isLoad;

	Effect*trailEffect;

protected:
	struct TrailDesc
	{
		float lifeTime;
		D3DXVECTOR3 startPos;
		D3DXVECTOR3 endPos;
		wstring diffuseMap;
		wstring alphaMap;
		int boneIdx;
	};

	vector<TrailDesc> trailDesc;
public:
	wstring&TypeName() { return typeName; }

public:
	virtual bool&LoadStart() { return isLoad; }
};