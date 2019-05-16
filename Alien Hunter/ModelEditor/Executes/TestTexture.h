#pragma once
#include "Systems/IExecute.h"

//   028_Instancing.fx

class TestTexture : public IExecute
{
public:
	void Initialize() override;
	void Ready() override;
	void Destroy() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override;

private:
	Effect * effect;

	ID3D11Buffer * vertexBuffer[2];
	
	Vertex vertices[6];

	D3DXCOLOR colors[2];
	D3DXMATRIX world[2][3];

	ID3D11Texture2D*worldData;
	ID3D11ShaderResourceView*worldDataSRV;
};