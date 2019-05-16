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

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    color = Diffuse;
    return color;
    //  �븻���� ���� ��ȯ
    //  Normal : -1 ~ 1
    //  Normal * 0.5f : -0.5 ~ 0.5
    //  Normal * 0.5f + 0.5f : 0 ~ 1
    //return float4((input.Normal * 0.5f) + 0.5f, 1);

}


