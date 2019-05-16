#include "000_header.hlsl"

struct PixelInput
{
//	�ش� �ȼ��� ��ġ
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};


//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    //  ��� == ����
    output.Position = mul(input.Position, World); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}


//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : �ڱ� �ڽ��� ��
//  Specular : ���ݻ籤, 
//  Diffuse + Ambient + Specular + Emissive(���� �� �ε巴�� ����)
float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    return color;
}


//  TODO : float�� ���������� �� ������� �˾ƺ���
//  TODO : EPSILON �˾ƺ���