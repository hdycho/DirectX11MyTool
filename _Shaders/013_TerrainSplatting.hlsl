#include "000_header.hlsl"


struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 BrushColor : COLOR0;
};

cbuffer AlphaValue:register(b10)
{
    int Type;
    float3 Location;
    int Range;
    float3 Color;
}


//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    //  행렬 == 공간
    output.Position = mul(input.Position, World); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = mul(input.Normal, (float3x3) World);

    output.BrushColor = Color;
    output.Uv = input.Uv;

    return output;
}

Texture2D ColorMap : register(t10);
SamplerState ColorSampler : register(s10);

Texture2D ColorMap2 : register(t11);
SamplerState ColorSampler2 : register(s11);

Texture2D ColorMap3 : register(t13);
SamplerState ColorSampler3 : register(s13);

Texture2D AlphaMap : register(t12);
SamplerState AlphaMapSampler : register(s12);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 colorMap = ColorMap.Sample(ColorSampler, input.Uv);
    float4 colorMap2 = ColorMap2.Sample(ColorSampler2, input.Uv);
    float4 colorMap3 = ColorMap3.Sample(ColorSampler3, input.Uv);
    float4 alphaMap = AlphaMap.Sample(AlphaMapSampler, input.Uv);

    
    float4 alpha = float4(alphaMap.r, alphaMap.r, alphaMap.r, alphaMap.r);
    float4 alpha2 = float4(alphaMap.g, alphaMap.g, alphaMap.g, alphaMap.g);
    //lerp => a*c+(1-c)*b의 식으로 선형보간함
    float4 temp = lerp(colorMap, colorMap2,alpha);
    float4 temp2 = lerp(temp, colorMap3, alpha2);

    DiffuseLighting(color, temp2, input.Normal);

    return color;
}
