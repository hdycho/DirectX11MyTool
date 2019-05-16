#pragma once
#include "Interface\IData.h"
#include "Interface\IPicked.h"

class Sky :public IData, public IPicked
{
public:
	Sky();
	~Sky();
	Sky(Sky&copy);

	void Initialize();
	void Ready();

	void Update();
	void PreRender();
	void Render();
	void RayleighMieRender();
	void PostRender();
	void ResizeScreen();

	//인터페이스
	wstring&Name() { return skyName; }
	wstring&LoadDataFile() { return loadFileName; }
	class GData* Save();
	void Load(wstring fileName);
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }

	bool IsPicked() { return isPick; }
	void SetPickState(bool val) { isPick = val; }

private:
	void GenerateSphere();
	void GenerateQuad();
	float GetStarIntensity();
	D3DXVECTOR3 GetDirection();
	D3DXCOLOR GetSunColor(float fTheta, int nTurbidity);
private:
	bool realTime;
	wstring skyName;
	wstring loadFileName;
	Texture*copyCloud = NULL;

	float theta, phi, setAngle;
	float prevTheta, prevPhi;

	Effect* effect;
	RenderTarget* mieTarget, *rayleighTarget;

	UINT radius, slices, stacks;

	VertexTexture* quadVertices;
	ID3D11Buffer* quadBuffer;

	Render2D* rayleigh2D;
	Render2D* mie2D;
	Render2D* noise2D;

	D3DXMATRIX world;

	Texture* starField;
	Texture* moon;
	Texture* moonGlow;
	Texture* cloud;

private:
	wstring loadStarMap;
	wstring loadMoonMap;
	wstring loadGlowMap;
	wstring loadcloudMap;
public:
	RenderTarget*&GetRayleigh() { return rayleighTarget; }
	RenderTarget*&GetMie() { return mieTarget; }

	Texture*&GetStarFieldMap() { return starField; }
	Texture*&GetMoonMap() { return moon; }
	Texture*&GetGlowMap() { return moonGlow; }
	Texture*&GetCloudMap() { return cloud; }

	float&Theta() { return theta; }
	float&Leans() { return setAngle; }
	float&TimeFactor() { return timeFactor; }
	float&StartTheta() { return startTheta; }
	Effect*GetEffect() { return effect; }
	D3DXVECTOR3&GetWaveLength() { return WaveLength; }
	int&GetSampleCount() { return SampleCount; }
	bool&IsShowLM() { return isShowLM; }

	float&CloudScale() { return cloudScale; }
	float&CloudSpeed() { return cloudSpeed; }
private:
	float timeFactor;
	float startTheta;
	D3DXVECTOR3 lightDirection;

	bool isPick;

private:
	float StarIntensity;
	float MoonAlpha;
	bool isShowLM;

	float cloudScale;
	float cloudSpeed;

	D3DXVECTOR3 WaveLength;
	int SampleCount;
	D3DXVECTOR3 InvWaveLength;
	D3DXVECTOR3 WaveLengthMie;

	UINT vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	// Reset은 9,10 때 디바이스 로스트가 발생했을 때 했었음.
	// 11부터는 안함.

private:
	ID3DX11EffectMatrixVariable*worldVariable;

	ID3DX11EffectScalarVariable*sampleVarialbe;
	ID3DX11EffectScalarVariable*coludScaleVariable;
	ID3DX11EffectScalarVariable*speedVariable;
	ID3DX11EffectScalarVariable*moonAlphaVariable;
	ID3DX11EffectScalarVariable*numTileVariable;
	ID3DX11EffectScalarVariable*noiseVariable;

	ID3DX11EffectVectorVariable*waveVariable;
	ID3DX11EffectVectorVariable*invWaveVariable;
	ID3DX11EffectVectorVariable*waveLengthMieVariable;

	ID3DX11EffectShaderResourceVariable*starFieldVariable;
	ID3DX11EffectShaderResourceVariable*moonVariable;
	ID3DX11EffectShaderResourceVariable*moonGlowVariable;
	ID3DX11EffectShaderResourceVariable*cloudVariable;

	ID3DX11EffectShaderResourceVariable*rayleighVariable;
	ID3DX11EffectShaderResourceVariable*meiVariable;

	//일단 안씀
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
};