#include "000_Header.hlsl"

struct PixelInput
{
                    //sementic
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

Texture2D Map2 : register(t1);
Texture2D Map3 : register(t2);

//���������� ����(�ϳ��ϳ�)
//�ȼ��� ��ġ��ȯ
//�Լ��� ���������� wvp��ȯ �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = mul(input.Normal, (float3x3)World);
    
    output.Uv = input.Uv;
    
    return output; // ��ȯ���� �ȼ��� ��ġ
}

//Ambient : �ڱ��ڽ��ǻ�
//Specular : ���ݻ籤
//Diffuse + Ambient + Emissive(���������ε巴��)

float4 PS(PixelInput input) : SV_TARGET
{
    //saturate : 0~1�� ������
    float4 t = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 t2 = Map2.Sample(DiffuseSampler, input.Uv);
    float4 alpha = Map3.Sample(DiffuseSampler, input.Uv);

    float4 diffuse = (1.0f - alpha.r) * t + t2 * alpha.r;

    float4 color = 0;
    DiffuseLight(color,diffuse, input.Normal);

    
    return color;
    //return float4((input.normal * 0.5f) + 0.5f,1); //�븻���͸� �������� ���̴����������� ����Ҽ����� 
}