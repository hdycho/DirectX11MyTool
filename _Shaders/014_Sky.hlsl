#include "000_header.hlsl"

cbuffer PS_Sky : register(b10)
{
    float4 Center;
    float4 Apex;

    float Height;
}

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float4 oPosition : POSITION0;
};


//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTexture input)
{
    PixelInput output;
    //  행렬 == 공간
    matrix world = Bones[BoneIndex];

    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.oPosition = input.Position;

    return output;
}


//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : 자기 자신의 색
//  Specular : 정반사광, 
//  Diffuse + Ambient + Specular + Emissive(색을 더 부드럽게 강조)
float4 PS(PixelInput input) : SV_TARGET
{
    float y = saturate(input.oPosition.y);

    return lerp(Center, Apex, y * Height);
}


//  TODO : float의 오차범위가 왜 생기는지 알아보기
//  TODO : EPSILON 알아보기