#pragma once
#include "Execute.h"

//   AlphaBlend : 한장의 이미지
//	BlendState로 조절
class TestAlphaBlend : public Execute
{
public:
	TestAlphaBlend(ExecuteValues* values);
	~TestAlphaBlend();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	Shader * shader;
	class MeshQuad * quad1;
	class MeshQuad * quad2;

	BlendState*blendState[9];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Selected = 0;
		}

		struct Struct
		{
			int Selected;

			float Padding[3];
		} Data;
	};
	Buffer* buffer;
};

