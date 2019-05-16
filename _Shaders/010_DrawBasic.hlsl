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

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    color = Diffuse;
    return color;
    //  노말값의 색상 변환
    //  Normal : -1 ~ 1
    //  Normal * 0.5f : -0.5 ~ 0.5
    //  Normal * 0.5f + 0.5f : 0 ~ 1
    //return float4((input.Normal * 0.5f) + 0.5f, 1);

}


