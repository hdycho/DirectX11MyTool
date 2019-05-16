#include "000_header.hlsl"

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};


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

    output.Uv = input.Uv;

    return output;
}


//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : 자기 자신의 색
//  Specular : 정반사광, 
//  Diffuse + Ambient + Specular + Emissive(색을 더 부드럽게 강조)
float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    return color;
}


//  TODO : float의 오차범위가 왜 생기는지 알아보기
//  TODO : EPSILON 알아보기