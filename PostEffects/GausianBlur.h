#pragma once

class GausianBlur :public Render2D
{
public:
	GausianBlur(ExecuteValues*values,string psName);
	~GausianBlur();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 0;
			Data.Height = 0;
			Data.Count = 1;
		}

		struct Struct
		{
			int Width;
			int Height;
			int Count;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};