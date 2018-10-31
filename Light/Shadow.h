#pragma once

//   depth buffer shadow
//   - PSM, LPPSM
//   - VSM(ue3���� ��� : �׸��ڰ� ����Ǵ� ���� ã�� ��)
//  - Cuscade(�Ÿ� ������ �ؼ� ����Ƽ�� �ٸ��� �ϴ� ���)

//   TODO : Models.h
//   TODO : GameAnimModel.cpp
//   TODO : GlobalBuffer.h
//   TODO : 000_header.hlsl
//   TODO : 043_Shadow.hlsl
//   TODO : Shadow.cpp

class Shadow
{
public:
	Shadow(ExecuteValues * values);
	~Shadow();

	void Add(class GameModel* obj);
	void Erase(wstring name);

	void ImGuiRender();
	void Update();
	void PreRender();
	void Render();
private:
	ExecuteValues * values;

	Shader *shaderDepth;
	Shader *shaderRender;

	RenderTarget * depthShadow;
	Render2D* depthRender;

	vector<class GameModel *> objs;

	class Perspective * perspective;

	ViewProjectionBuffer * vpBuffer;

	SamplerState * samplerState[2];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Bias = 0.0005f;
			Data.Selected = 2;
		}

		struct Struct
		{
			D3DXVECTOR2 Size;
			float Bias;
			int Selected;

		} Data;
	};

	Buffer * buffer;
};