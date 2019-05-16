#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Width;
    int Height;
    int Size;
}


SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

PixelInput VS(VertexTexture input)
{
    PixelInput output;
   
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
    //화면 크기비율
    float xRatio = 1.0f / (float) Width;
    float yRatio = 1.0f / (float) Height;

    //외부에서 설정한 사이즈를 비율로나눔
    float2 mosaicRatio;
    mosaicRatio.x = Size * xRatio;
    mosaicRatio.y = Size * yRatio;

    //비율의 중앙점
    float2 mosaicCenter;
    mosaicCenter.x = mosaicRatio.x / 2.0f;
    mosaicCenter.y = mosaicRatio.y / 2.0f;

    //모자이크uv좌표비율
    //모자이크비율에 맞춰서 실제사이즈값 나옴
    float2 mosaicUvRatio;
    mosaicUvRatio.x = (int)(input.Uv.x / mosaicRatio.x);
    mosaicUvRatio.y = (int)(input.Uv.y / mosaicRatio.y);

    //실제 uv좌표위치가 속한 비율영역의 중앙좌표
    float2 uvCenter = mosaicUvRatio * mosaicRatio + mosaicCenter;
     
    float4 color = Map.Sample(Sampler, uvCenter);
   
    return color;
}
