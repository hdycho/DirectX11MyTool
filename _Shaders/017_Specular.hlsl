#include "000_header.hlsl"


//clip�Լ�
//���� 0�����̸� �ش��ȼ��� �׸����ʴ´�
//PS �߰��� ó��

struct PixelInput
{
//	�ش� �ȼ��� ��ġ
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 ViewDir : VIEWDIR0;
};


//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];
    //  ��� == ����
    output.Position = mul(input.Position, world); //  ���� ����
    output.ViewDir = WorldViewDirection(output.Position); // ��ü�� ���ؽ��� ī�޶� �ٶ󺸴� ����

    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.Uv = input.Uv;
    output.Normal = WorldNormal(input.Normal, world); // ���� �븻���� ���庯ȯ �Ѱ�

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



