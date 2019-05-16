#include "000_header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};



PixelInput VS(VertexTexture input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Uv = input.Uv;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(DiffuseSampler, input.Uv);


    return float4(color);
}
