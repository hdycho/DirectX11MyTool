#include "000_header.hlsl"

struct PixelInput
{
//	�ش� �ȼ��� ��ġ
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 ViewDir : VIEWDIR0;
    float3 Tangent : TANGENT0;
};

//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    matrix world = 0;

   [branch]
    if (UseBlend == true)
        world = SkinWorld(input.BlendIndices, input.BlendWeights);
    else
        world = BoneWorld();

    //  ��� == ����
    output.Position = mul(input.Position, world); //  �d�� ����
    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

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

    bool isNormal = false;
    bool isSpecular = false;
    
    if (isVisible == 1)
    {
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        if (normal.r!=0)
            isNormal = true;

        if(isNormal)
             NormalMapping(color, normal, input.Normal, input.Tangent);
    }


    float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    if(specular.r!=0)
        isSpecular = true;

    if(isSpecular)
        SpecularLighting(color, specular, input.Normal, input.ViewDir);

    return color;
    //  �븻���� ���� ��ȯ
    //  Normal : -1 ~ 1
    //  Normal * 0.5f : -0.5 ~ 0.5
    //  Normal * 0.5f + 0.5f : 0 ~ 1
    //return float4((input.Normal * 0.5f) + 0.5f, 1);

}


//  TODO : float�� ���������� �� ������� �˾ƺ���
//  TODO : EPSILON �˾ƺ���