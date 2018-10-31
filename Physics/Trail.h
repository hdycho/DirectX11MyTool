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

	bool bBone;		//������ �Է����
	bool bCull;		//�ø��Ű����
	bool bDepth;	//���̹��۾�����

	bool bRender;	//������ �Ⱥ�����

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
			D3DXMATRIX Worlds[128];	//Ʈ���� ����

			int Count;				//Ʈ���� ����
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
