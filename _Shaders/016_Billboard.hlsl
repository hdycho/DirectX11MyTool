#include "000_header.hlsl"


//clip함수
//값이 0이하이면 해당픽셀을 그리지않는다
//PS 중간에 처리

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTexture input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];
    //  행렬 == 공간
    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    clip(color.a - 0.9f); // 이값이 0보다작아지면 밑에껀 실행안함

    return color;
}



