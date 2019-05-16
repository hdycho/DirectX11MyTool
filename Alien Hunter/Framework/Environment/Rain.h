#pragma once
#include "Interface\IData.h"
#include "Interface\IPicked.h"

class Rain :public IData, public IPicked
{
private:
	struct VertexRain;

public:
	Rain(D3DXVECTOR3& size, UINT count);
	Rain(Rain&copy);
	~Rain();

	void Ready();

	void Update();
	void Render();

	void ImGuiRender();
	void UpdateVertexResource();

public:
	class GData* Save() override;
	wstring&Name() override;
	wstring&LoadDataFile() { return loadFileName; }
	void Load(wstring fileName) override;
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }

	bool IsPicked() override;
	void SetPickState(bool val) override;

private:
	Effect			*effect;

	ID3D11Buffer	*vertexBuffer;
	VertexRain * vertices;


	TextureArray	*rainMaps;
public:
	UINT			particleCount;
	D3DXVECTOR3 size;
	D3DXVECTOR3 velocity;
	float drawDist;
	D3DXCOLOR color;

	bool isPicked;
	wstring name;
	wstring loadFileName;
private:
	struct VertexRain
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Scale;
		UINT TextureId;
		float Random;

		VertexRain() : Position(0, 0, 0), Scale(0, 0), TextureId(0), Random(0.0f) {}
		VertexRain(D3DXVECTOR3& position, D3DXVECTOR2& scale, UINT& textureId, float& random)
			: Position(position), Scale(scale), TextureId(textureId), Random(random) {}
	};

private:
	ID3DX11EffectShaderResourceVariable*srvVariable;
	ID3DX11EffectVectorVariable*sizeVariable;
	ID3DX11EffectVectorVariable*velocityVariable;
	ID3DX11EffectScalarVariable*distVariable;
	ID3DX11EffectVectorVariable*colorVariable;
	ID3DX11EffectVectorVariable*originVariable;
	ID3DX11EffectScalarVariable*timeVariable;

	//¿œ¥‹ æ»æ∏
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
};