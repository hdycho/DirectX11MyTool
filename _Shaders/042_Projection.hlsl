#include "000_header.hlsl"

cbuffer VS_Project : register(b10)
{
    matrix V;
    matrix P;
}


struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 pPosition : POSITION0;
};



PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    //외부에서 설정한 카메라 공간으로 변화된 포지션
    output.pPosition = mul(input.Position, world);
    //외부에서 설정한 뷰프로젝션으로 맞춰서 변환
    output.pPosition = mul(output.pPosition, V);
    output.pPosition = mul(output.pPosition, P);

    return output;
}



float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color ,diffuse,input.Normal);

    float2 projUv = 0;

    //-1~1을 0~1 UV좌표로 바꿈
    projUv.x = input.pPosition.x / input.pPosition.w * 0.5f + 0.5f;
    projUv.y = input.pPosition.y / input.pPosition.w * 0.5f + 0.5f;

    //화면에 들어가있는지 판별
    //화면밖으로 벗어나면 -1~1사이 밖으로 넘어감
    if(saturate(projUv.x) == projUv.x&&
       saturate(projUv.y) == projUv.y)
    {
        float4 projMap = NormalMap.Sample(NormalSampler, projUv);
        
        color = projMap;
    }

    return color;
}
