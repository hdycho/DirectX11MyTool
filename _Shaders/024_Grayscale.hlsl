#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Select;
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
    
    [branch]
    if (Select == 0)
    {
        //평균회색색상
        color.rgb = (color.r + color.g + color.b) / 3;
    }
    else if (Select == 1)
    {
        //회색색상
        float3 monotone = float3(0.299f, 0.587f, 0.114f);
        color.rgb = dot(color.rgb, monotone);
    }
    else if (Select == 2)
    {
        //반전효과
        color.rgb = saturate(1.0f - color.rgb);
    }
        
    return color;
}