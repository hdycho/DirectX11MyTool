#include "001_header2.hlsl"

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

Texture2D Map : register(t10);
SamplerState Sampler : register(s10);

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);

    return color;
}