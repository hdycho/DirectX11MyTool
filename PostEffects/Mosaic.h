#pragma once

class Mosaic :public Render2D
{
public:
	Mosaic(ExecuteValues*values);
	~Mosaic();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 0;
			Data.Height = 0;
			Data.Size = 1;
		}

		struct Struct
		{
			int Width;
			int Height;
			int Size;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};