#include "000_header.hlsl"

cbuffer PS_Normal : register(b10)
{
    int isVisible;
}

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float4 wPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 ViewDir : VIEWDIR0;
    float3 Tangent : TANGENT0;
};

//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTextureNormalTangent input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];
    //  행렬 == 공간
    output.Position = mul(input.Position, world); //  웓드 공간
    output.wPosition = output.Position;
    
    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Uv = input.Uv;
    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);
    
    if (isVisible == 1)
    {
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, normal, input.Normal, input.Tangent);
    }
    
    float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    SpecularLighting(color, specular, input.Normal, input.ViewDir);

    int i = 0;
    for (i = 0; i < PointLightCount; i++)
        PointLighting(color, PointLights[i], input.wPosition, input.Normal);

    for (i = 0; i < SpotLightCount; i++)
        SpotLighting(color, SpotLights[i], input.wPosition, input.Normal);

    color.a = 1.0f;
    return color;
}



