#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    float4 Gamma;
}


SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

PixelInput VS(VertexTexture input)
{
    PixelInput output;
   
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);

    color.r = pow(color.r, Gamma.r);
    color.g = pow(color.g, Gamma.g);
    color.b = pow(color.b, Gamma.b);

    color.a = 1.0f;
   
    return color;
}
