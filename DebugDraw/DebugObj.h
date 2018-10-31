#pragma once

class DebugObj
{
public:
	DebugObj(UINT vCount, UINT iCount);
	~DebugObj();

	virtual void Update(D3DXMATRIX parentMatrix) = 0;
	virtual void Render() = 0;
	virtual void ChangeColor(D3DXCOLOR color) {}

	inline D3DXCOLOR&GetColor() { return colorBuffer->Data.color; }
protected:
	class ColorBuffer :public ShaderBuffer
	{
	public:
		ColorBuffer()
			:ShaderBuffer(&Data, sizeof(Data))
		{
			Data.color = D3DXCOLOR(1, 0, 0, 1);
		}

		struct Struct
		{
			D3DXCOLOR color;
		}Data;
	};
	ColorBuffer*colorBuffer;
protected:
	Material*material;
	WorldBuffer*worldBuffer;

	ID3D11Buffer*vertexBuffer;
	ID3D11Buffer*indexBuffer;

	UINT vertexCount;
	UINT indexCount;

	VertexColor*vertices;
	UINT*indices;
};