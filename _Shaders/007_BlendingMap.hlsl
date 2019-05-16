#include "000_Header.hlsl"

struct PixelInput
{
                      //SV만 정해진 예약어임-> 시멘틱
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};


Texture2D Map2 : register(t1);
Texture2D Map3 : register(t2);


PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = mul(input.Normal, (float3x3)World);

    output.Uv = input.Uv;

    return output; // 반환값이 픽셀의 위치
}


float4 PS(PixelInput input):SV_TARGET
{
    float4 t1 = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 t2 = Map2.Sample(DiffuseSampler, input.Uv);
    float4 alpha = Map3.Sample(DiffuseSampler, input.Uv);
    
    float4 ambient = (1 - alpha) * t1 + alpha * t2; // 물체의 색

    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    DiffuseLight(color, ambient, input.Normal);

    return color;
}