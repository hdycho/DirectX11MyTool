#pragma once

class Gamma :public Render2D
{
public:
	Gamma(ExecuteValues*values);
	~Gamma();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Gamma = { 1,1,1,1 };
		}

		struct Struct
		{
			D3DXCOLOR Gamma;
		} Data;
	};
	Buffer* buffer;
};