#include "000_header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};



PixelInput VS(VertexTexture input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.Uv = input.Uv;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(DiffuseSampler, input.Uv);


    return float4(color);
}
