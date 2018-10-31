#pragma once

#include "../Objects/GameRender.h"

class Trail : public GameRender
{
public:
	Trail();
	virtual ~Trail();

	void Update();
	virtual void Render();
	void ImGuiRender();

	wstring&GetTrailName() { return trailName; }
	wstring&GetParentName() { return parentName; }
	void SetTrailCount(int count) { vsBuffer->Data.Count = count; }
	void SetRender(bool bRender) { this->bRender = bRender; }
	void SetGameModel(class GameModel * model){ this->model = model; }
	void SetBoneName(wstring name) { boneName = name; bBone = true; }
	void ShiftMatrix();

private:
	WorldBuffer* worldBuffer;
	ID3D11Buffer* vertexBuffer;
	Shader* shader;

	Texture* texture;
	RasterizerState* cullMode[2];
	DepthStencilState* depthMode[2];
	BlendState* blendMode[2];

	class GameModel* model;

	wstring boneName;
	wstring trailName;
	wstring parentName;

	D3DXMATRIX worldMatrix;

	D3DXVECTOR3 setPosition;
	D3DXVECTOR3 setRotation;
	D3DXVECTOR3 setScale;

	bool bBone;		//뼈정보 입력됬는지
	bool bCull;		//컬모드키는지
	bool bDepth;	//깊이버퍼쓰는지

	bool bRender;	//보일지 안보일지

	float elapsedTime;
	float deltaTime;
private:
	class VsBuffer : public ShaderBuffer
	{
	public:
		VsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++) {
				D3DXMatrixIdentity(&Data.Worlds[i]);
			}

			Data.Count = 1;
		}

		struct Struct
		{
			D3DXMATRIX Worlds[128];	//트레일 월드

			int Count;				//트레일 갯수
			float Padding[3];
		} Data;
	};

	VsBuffer* vsBuffer;

	class PsBuffer : public ShaderBuffer
	{
	public:
		PsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 0, 0, 1);

			Data.UseTexture = 1;
		}

		struct Struct
		{
			D3DXCOLOR Color;

			int UseTexture;
			float Padding[3];
		} Data;
	};

	PsBuffer* psBuffer;
};
