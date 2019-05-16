#include "000_header.hlsl"


struct PixelInput
{
//	�ش� �ȼ��� ��ġ
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


//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    //  ��� == ����
    output.Position = mul(input.Position, World); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

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
    //lerp => a*c+(1-c)*b�� ������ ����������
    float4 temp = lerp(colorMap, colorMap2,alpha);
    float4 temp2 = lerp(temp, colorMap3, alpha2);

    DiffuseLighting(color, temp2, input.Normal);

    return color;
}
