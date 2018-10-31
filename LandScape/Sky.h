#pragma once
//   MultiTexture : 두장의 이미지가 동시에 들어갈 때

//   TODO : sky.h, .cpp
//   TODO : 044_Scattering.hlsl

class Sky
{
public:
	Sky(ExecuteValues* values);
	~Sky();

	void Update();
	void PreRender();
	void Render();

private:
	D3DXVECTOR3 GetDirection();
	D3DXCOLOR GetSunColor(float fTheta, int nTurbidity);

	void GenerateSphere();
	void GenerateQuad();
	void GenerateMoon();
	void GenerateCloud();

	float GetStarIntensity();

	void DrawMoon();
	void DrawGlow();
	void DrawCloud();
private:
	float timeFactor;
	ExecuteValues *values;
	bool realTime;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget *mieTarget, *rayLeighTarget;
	Render2D*rayleigh2D, *mie2D;
	DepthStencilState * depthState[2];

	Shader *scatterShader, *targetShader, *moonShader, *cloudShader;

	Texture*starField, *moonTex, *glowTex, *permTex;

	UINT vertexCount, indexCount;
	UINT radius, slices, stacks;

	ID3D11Buffer *vertexBuffer, *indexBuffer;

	VertexTexture *quadVertices;
	ID3D11Buffer *quadBuffer;

	ID3D11Buffer*moonVBuffer, *moonIBuffer;

	WorldBuffer * worldBuffer;

	SamplerState*rayleighSampler;
	SamplerState*mieSampler;
	SamplerState*starSampler;
	SamplerState*moonSampler;
	SamplerState*cloudSampler;

	BlendState*moonBlend[2];

	RasterizerState* rasterizerState[2];

private:
	class PS_ScatterBuffer : public ShaderBuffer
	{
	public:
		PS_ScatterBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.StarIntensity = 0.0f;
			Data.Running = 0.0f;
		}

		struct Struct
		{
			float StarIntensity;
			float Running;
			float padding[2];

			D3DXCOLOR SunColor;

		} Data;
	};

	PS_ScatterBuffer * scatterBuffer;

	class TargetBuffer : public ShaderBuffer
	{
	public:
		TargetBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.WaveLength = D3DXVECTOR3(0.65f, 0.57f, 0.475f);
			Data.SampleCount = 20;

			Data.InvWaveLength.x = 1.0f / powf(Data.WaveLength.x, 4);
			Data.InvWaveLength.y = 1.0f / powf(Data.WaveLength.y, 4);
			Data.InvWaveLength.z = 1.0f / powf(Data.WaveLength.z, 4);

			Data.WaveLengthMie.x = powf(Data.WaveLength.x, -0.84f);
			Data.WaveLengthMie.y = powf(Data.WaveLength.y, -0.84f);
			Data.WaveLengthMie.z = powf(Data.WaveLength.z, -0.84f);
		}

		struct Struct
		{
			D3DXVECTOR3 WaveLength;
			int SampleCount;

			D3DXVECTOR3 InvWaveLength;
			float Padding2;

			D3DXVECTOR3 WaveLengthMie;
			float Padding3;
		} Data;
	};

	TargetBuffer * targetBuffer;

	class VS_CloudBuffer : public ShaderBuffer
	{
	public:
		VS_CloudBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.NumTiles = 0.0f;
		}

		struct Struct
		{
			float NumTiles;
			float padding[3];

		} Data;
	};

	VS_CloudBuffer * cloudBuffer;
};