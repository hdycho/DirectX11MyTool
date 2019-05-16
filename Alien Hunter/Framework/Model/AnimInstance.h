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

	//���ϸ����� Ŭ������ AddClip�κп��� ȣ�����ָ��
	void AddClip(class ModelClip*clip);

	class Model*GetModel();
public:
	void AddWorld(class GameAnimator*anim);
	void DeleteWorld(UINT instanceID);

	//���Ӹ�,���ӿ��ϸ����Ϳ� UpdateWorld���� ȣ�����ָ��
	void UpdateWorld(UINT instanceNumber, D3DXMATRIX&world);

	//Ŭ�������Ҷ�
	void StartClip(UINT instanceNumber);

	//Ŭ���ʱ�ȭ�Ҷ�
	void InitClip(UINT instanceNumber);

	//���ϸ��̼� ������ ���ϸ��̼� ����� ����
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

	//�ش� ���ϸ����� Ŭ�������� Ŭ�����簡 �ʿ��ϱ⶧��
	vector<class GameAnimator*> anims;

	//���ϸ��̼� Ŭ���� ���ڵ��� �۵��ϰ� �ϰ� �ߺ�����
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
		UINT playClipIdx; // 0���ε����� T������
		UINT nextClipIdx; // ����ƴҽ� �۵�����
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