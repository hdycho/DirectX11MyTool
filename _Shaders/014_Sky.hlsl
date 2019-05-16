#include "000_header.hlsl"

cbuffer PS_Sky : register(b10)
{
    float4 Center;
    float4 Apex;

    float Height;
}

struct PixelInput
{
//	�ش� �ȼ��� ��ġ
    float4 Position : SV_POSITION;
    float4 oPosition : POSITION0;
};


//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTexture input)
{
    PixelInput output;
    //  ��� == ����
    matrix world = Bones[BoneIndex];

    output.Position = mul(input.Position, world); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.oPosition = input.Position;

    return output;
}


//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : �ڱ� �ڽ��� ��
//  Specular : ���ݻ籤, 
//  Diffuse + Ambient + Specular + Emissive(���� �� �ε巴�� ����)
float4 PS(PixelInput input) : SV_TARGET
{
    float y = saturate(input.oPosition.y);

    return lerp(Center, Apex, y * Height);
}


//  TODO : float�� ���������� �� ������� �˾ƺ���
//  TODO : EPSILON �˾ƺ���