#pragma once

class AnimInstance
{
public:
	AnimInstance(class Model * model, wstring shaderFile);
	AnimInstance(class Model * model, Effect*effect);
	~AnimInstance();

	void Ready();
	void Update();
	void Render(UINT tec = 0);

	//에니메이터 클래스에 AddClip부분에서 호출해주면됨
	void AddClip(class ModelClip*clip);

	class Model*GetModel();
public:
	void AddWorld(class GameAnimator*anim);
	void DeleteWorld(UINT instanceID);

	//게임모델,게임에니메이터에 UpdateWorld에서 호출해주면됨
	void UpdateWorld(UINT instanceNumber, D3DXMATRIX&world);

	//클립시작할때
	void StartClip(UINT instanceNumber);

	//클립초기화할때
	void InitClip(UINT instanceNumber);

	//에니메이션 블랜딩및 에니메이션 재생시 사용됨
	void UpdateAnimation(UINT instanceNumber, UINT curFrame, UINT nextFrame, float frameTime, float Time);
	void UpdateBlending(UINT instanceNumber, UINT curFrame, UINT nextFrame, float blendTime, UINT nextClipIdx);

	void ChangeDiffuse(UINT instanceNumber, D3DXCOLOR color);

private:
	struct RenderDesc
	{
		float isRender;
		float padding[3];

		RenderDesc()
		{
			isRender = false;
		}
	};
	RenderDesc rDesc[64];
public:
	void SetRenderState(int instNum, bool state);
	float GetRenderState(int instNum) { return rDesc[instNum].isRender; }

private:
	class Model * model;

	//해당 에니메이터 클래스에서 클립복사가 필요하기때문
	vector<class GameAnimator*> anims;

	//에니메이션 클립이 각자따로 작동하게 하고 중복방지
	vector<wstring> clipNames;

	UINT maxCount;

	vector<ID3D11Texture2D *> transTextures;
	TextureArray*textureArray;

	D3DXMATRIX worlds[64];

	struct FrameDesc
	{
		UINT CurFrame;
		UINT NextFrame;
		float Time;
		float FrameTime;
		float BlendTime;
		UINT playClipIdx; // 0번인덱스는 T포즈임
		UINT nextClipIdx; // 블랜드아닐시 작동안함
		float padding;
		D3DXCOLOR diffuse;
	};
	FrameDesc frames[64];

private:
	vector<ID3DX11EffectVariable*> frameDescVars;
	vector<ID3DX11EffectVariable*> renderVars;
	UINT maxFrameCount = 0;

private:
	vector<ID3DX11EffectVectorVariable*> frustumVars;
	class Frustum*frustum;
	D3DXPLANE plane[6];
};