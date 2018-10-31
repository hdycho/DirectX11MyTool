#pragma once

class Sharpening :public Render2D
{
public:
	Sharpening(ExecuteValues*values);
	~Sharpening();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 0;
			Data.Height = 0;
			Data.Level = 0;
		}

		struct Struct
		{
			int Width;
			int Height;
			int Level;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};