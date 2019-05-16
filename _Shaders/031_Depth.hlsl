#include "000_header.hlsl"

//clip함수
//값이 0이하이면 해당픽셀을 그리지않는다
//PS 중간에 처리

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float4 pPosition : POSITION0;
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
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.pPosition = output.Position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float depth = input.pPosition.z/input.pPosition.w;
    
    float4 color = 0;

    [branch]
    if (depth < 0.9f)
    {
        color = float4(1, 0, 0, 1);
    }
    else if (depth > 0.925f)
    {
        color = float4(0, 0, 1, 1);
    }
    else if (depth > 0.9f)
    {
        color = float4(0, 1, 0, 1);
    }

    return color;
}



