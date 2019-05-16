#pragma once


class TrailRenderer
{
public:
	TrailRenderer(class GameModel * model, Effect * effect);
	~TrailRenderer();

	void Ready();
	void Update();
	void PreRender();
	void Render();
	void ImGuiRender();
	void PostRender();
	void ResizeScreen();

	void SetBone(int val) { boneIndex = val; }
	int GetBone() { return boneIndex; }
	void SetLifeTime(float val) { lifeTime = val; }
	float GetLifeTime() { return lifeTime; }
	D3DXMATRIX&StartPos() { return startPos; }
	D3DXMATRIX&EndPos() { return endPos; }
	Texture*&GetDiffuseMap() { return diffuse; }
	Texture*&GetAlphaMap() { return alpha; }

	void SetRun(bool val) { run = val; }
	void ResetTrail();
	wstring&Name() { return name; }

private:
	void MakeRomLines();
	void CreateTexture();

	void CreateBuffer();

private:
	void LoadTexture(wstring file, UINT type);

private:
	struct MyVertex
	{
		D3DXVECTOR3	Position = D3DXVECTOR3(0, 0, 0);
		float Time = 0;
	};

	wstring		name;
	bool		run;
	float		count;
	float		lifeTime;

	Effect		*effect;
	Effect		*lineEffect;

	Texture		*diffuse;
	Texture		*alpha;

	//	모델이랑 본 관련
	int					boneIndex;
	D3DXMATRIX			boneMatrix;
	class GameModel	*model;

	//	startPos : 무기 끝 지점
	//	endPos : 손잡이 쪽
	D3DXMATRIX			startPos;
	D3DXMATRIX			endPos;

	//	CatMullRom 해서 보간한 좌표들
	vector<D3DXVECTOR3>	start;
	vector<D3DXVECTOR3>	end;

	//	보간 하기전 좌표들
	vector<MyVertex>	linePoints[2];

	VertexTextureNormal	*vertices;
	UINT				*indices;
	ID3D11Buffer		*vertexBuffer, *indexBuffer;
	UINT				indexCount, vertexCount;

	class LineMaker*lines;
	class DebugLine*debugLine[2];

	bool isLoad;
	bool isModel; // 뼈한개 모델
public:
	bool SetLoad(bool val) { isLoad = val; }
private:
	ID3DX11EffectShaderResourceVariable * trailMapVar;
	ID3DX11EffectShaderResourceVariable * alphaMapVar;
};