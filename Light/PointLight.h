#pragma once

class PointLight
{
private:
	bool isVisible;
public:
	PointLight();
	~PointLight();

	void Update();
	void Render();
public:
	struct Desc
	{
		D3DXVECTOR3 Position;
		float Range;
		D3DXVECTOR3 Color;
		float Intensity;
	};
private:
	class Buffer :public ShaderBuffer
	{
	public:
		Buffer() :ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 32; i++)
			{
				Data.Lights[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Color = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Intensity = 0;
				Data.Lights[i].Range = 0;
			}
			Data.Count = 0;
		}
		struct Struct
		{
			Desc Lights[32];

			UINT Count;
			float Padding[3];
		}Data;
	};
	Buffer*buffer;
public:
	void Add(Desc&desc);
	void Remove();
	void IsDebugVisible();
};