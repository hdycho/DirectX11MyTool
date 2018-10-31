#pragma once

class Embossing :public Render2D
{
public:
	Embossing(ExecuteValues*values);
	~Embossing();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 0;
			Data.Height = 0;
		}

		struct Struct
		{
			int Width, Height;
			float Padding[2];
		} Data;
	};
	Buffer* buffer;
};