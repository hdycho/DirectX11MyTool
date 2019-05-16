#include "000_header.hlsl"

cbuffer PS_Select : register(b10)
{
    int Selected;
}


struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
};



PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = WorldNormal(input.Normal, world);
    
    output.Uv = input.Uv;
    return output;
}



float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    DiffuseLighting(color, input.Normal);

    return color;
}
