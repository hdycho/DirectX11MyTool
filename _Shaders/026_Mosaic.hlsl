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
    return output; // ��ȯ���� �ȼ��� ��ġ
}

float4 PS(PixelInput input) : SV_TARGET
{
    //ȭ�� ũ�����
    float xRatio = 1.0f / (float) Width;
    float yRatio = 1.0f / (float) Height;

    //�ܺο��� ������ ����� �����γ���
    float2 mosaicRatio;
    mosaicRatio.x = Size * xRatio;
    mosaicRatio.y = Size * yRatio;

    //������ �߾���
    float2 mosaicCenter;
    mosaicCenter.x = mosaicRatio.x / 2.0f;
    mosaicCenter.y = mosaicRatio.y / 2.0f;

    //������ũuv��ǥ����
    //������ũ������ ���缭 ��������� ����
    float2 mosaicUvRatio;
    mosaicUvRatio.x = (int)(input.Uv.x / mosaicRatio.x);
    mosaicUvRatio.y = (int)(input.Uv.y / mosaicRatio.y);

    //���� uv��ǥ��ġ�� ���� ���������� �߾���ǥ
    float2 uvCenter = mosaicUvRatio * mosaicRatio + mosaicCenter;
     
    float4 color = Map.Sample(Sampler, uvCenter);
   
    return color;
}
