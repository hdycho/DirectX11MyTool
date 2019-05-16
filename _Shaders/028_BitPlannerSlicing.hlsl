#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int bitValue;
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
    uint4 bitColor = color * 255;
    
    bitColor >>= bitValue;
    bitColor <<= bitValue;

    color = (float4)bitColor / 255.0f;

    return color;
}
