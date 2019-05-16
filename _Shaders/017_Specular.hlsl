#include "000_header.hlsl"


//clip함수
//값이 0이하이면 해당픽셀을 그리지않는다
//PS 중간에 처리

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 ViewDir : VIEWDIR0;
};


//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];
    //  행렬 == 공간
    output.Position = mul(input.Position, world); //  월드 공간
    output.ViewDir = WorldViewDirection(output.Position); // 물체의 버텍스가 카메라를 바라보는 방향

    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Uv = input.Uv;
    output.Normal = WorldNormal(input.Normal, world); // 모델의 노말값을 월드변환 한것

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    float specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    SpecularLighting(color, specular, input.Normal, input.ViewDir);

    //Color= Diffuse + Specular

    color.a = 1.0f;
    return color;
}



