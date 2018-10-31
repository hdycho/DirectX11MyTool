#pragma once

class BitPlannerSlicing :public Render2D
{
public:
	BitPlannerSlicing(ExecuteValues*values);
	~BitPlannerSlicing();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.bitValue = 1;
		}

		struct Struct
		{
			int bitValue;
			float Padding[3];
		} Data;
	};
	Buffer* buffer;
};