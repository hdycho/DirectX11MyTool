#include "000_Header.hlsl"

struct PixelInput
{
                    //sementic
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

//���������� ����(�ϳ��ϳ�)
//�ȼ��� ��ġ��ȯ
//�Լ��� ���������� wvp��ȯ �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];

    output.Position = mul(input.Position, world);
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
    
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    [branch]
    if (length(diffuse) < 0.0f)
        DiffuseLighting(color, input.Normal);
    else
        DiffuseLighting(color, diffuse,input.Normal);

    return color;


    //return float4((input.normal * 0.5f) + 0.5f,1); //�븻���͸� �������� ���̴����������� ����Ҽ����� 
}