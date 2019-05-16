#pragma once

class TerrainRender
{
private:
	struct TerrainSRT
	{
		D3DXVECTOR3 scale;
		D3DXVECTOR3 rotate;
		D3DXVECTOR3 trans;
	};
	TerrainSRT terrainSrt;

public:
	static const float MaxDistance;
	static const float MinDistance;
	static const float MaxTessellation;
	static const float MinTessellation;

public:
	TerrainRender(class Terrain* terrain);
	~TerrainRender();

	void Initialize();
	void Ready(Material*material);
	void ChangeImageMap(wstring fileName, int type);

	void AdjustY(Effect*effect, ID3D11ShaderResourceView*srv, ID3D11UnorderedAccessView*uav);
	void BlendSet();

	void Render();
	void RenderShadow(UINT tech, UINT pass);

	TerrainSRT&GetSRT() { return terrainSrt; }
	ID3D11ShaderResourceView*GetLayerView() { return layerMapArray->GetView(); }
private:
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT row, UINT col);
	void BuildQuadPatchVB();
	void BuildQuadPatchIB();

	void CreateBlendMap();
	void SmoothBlendMap(vector<D3DXCOLOR>& colors);

private:
	D3DXMATRIX world;
	ID3D11Buffer* cBuffer;

	class Sky*sky;
	class Terrain* terrain;

	ID3D11Buffer* quadPatchVB;
	ID3D11Buffer* quadPatchIB;
	class TerrainCP* patchVertices;

	vector<class Patch *> patches;

	TextureArray* layerMapArray;

	ID3D11ShaderResourceView* layerMapArraySRV;
	ID3D11ShaderResourceView* blendMapSRV;
	ID3D11ShaderResourceView* heightMapSRV;

	UINT patchVerticesCount;
	UINT patchQuadFacesCount;
	UINT patchVertexRows;
	UINT patchVertexCols;

	Material* material;
	vector<D3DXVECTOR2> patchBoundsY;

	class Frustum* frustum;
	int bWireFrame = 0;

private:
	RenderTarget*renderTargetView;

	ID3D11RenderTargetView* rtvs[2];
	ID3D11DepthStencilView* dsv;
	Viewport*view;
public:
	ID3D11Texture2D* GetRenderTargetTexture() { return renderTargetView->GetTexture(); }
	ID3D11ShaderResourceView*GetHeightMapSRV() { return heightMapSRV; }
	int&WireFrameMode() { return bWireFrame; }
	ID3D11Buffer*GetVertexBuffer() { return quadPatchVB; }
	class TerrainCP*GetVertices() { return patchVertices; }
	int GetVertexCount() { return patchVerticesCount; }

private:
	struct Buffer
	{
		D3DXCOLOR FogColor;

		float FogStart;
		float FogRange;

		float MinDistance;
		float MaxDistance;
		float MinTessellation;
		float MaxTessellation;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace;

		int TexScale = 32;
		float Padding[2];

		D3DXPLANE WorldFrustumPlanes[6];

	} buffer;
public:
	Buffer&GetBuffer() { return buffer; }

private:
	ID3DX11EffectShaderResourceVariable*heightMapVariable;
	ID3DX11EffectShaderResourceVariable*layerMapVariable;
	ID3DX11EffectShaderResourceVariable*blendMapVariable;

	ID3DX11EffectVectorVariable*blendPosVariable;
	ID3DX11EffectVectorVariable*activeVariable;
	ID3DX11EffectScalarVariable*maxHeightVariable;
	ID3DX11EffectScalarVariable*detailPowerVariable;
	ID3DX11EffectScalarVariable*detailValueVariable;

	ID3DX11EffectConstantBuffer*terrainVariable;
};
