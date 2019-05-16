#include "000_header.hlsl"

cbuffer VS_Trail : register(b7)
{
    matrix TrailWorlds[128];
    int TrailCount;         //  �ִ� Ʈ���ϰ���
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

SamplerState Sampler : register(s5); // sampler�� ���� �ƴ϶� s0
// �⺻�� �ᵵ �� �ȳ־����� �⺻�� ����
Texture2D Map : register(t5); // texture�� t0

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    float rate = 1.0f / (float) TrailCount;
    int index = (int) (input.Uv.x / rate);

    index = min(index, TrailCount - 1);
    matrix world = TrailWorlds[index];

    //output.Position = mul(input.Position, World);
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

cbuffer PS_Trail : register(b7)
{
    float4 TrailColor;

    int TrailSelect;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = TrailColor;
    color.a = 1.0f - input.Uv.y;

    if (TrailSelect == 1)
		color = Map.Sample(Sampler, input.Uv);  
        
	return color;
}