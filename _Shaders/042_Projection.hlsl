#include "000_header.hlsl"

cbuffer VS_Project : register(b10)
{
    matrix V;
    matrix P;
}


struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 pPosition : POSITION0;
};



PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    //�ܺο��� ������ ī�޶� �������� ��ȭ�� ������
    output.pPosition = mul(input.Position, world);
    //�ܺο��� ������ �������������� ���缭 ��ȯ
    output.pPosition = mul(output.pPosition, V);
    output.pPosition = mul(output.pPosition, P);

    return output;
}



float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color ,diffuse,input.Normal);

    float2 projUv = 0;

    //-1~1�� 0~1 UV��ǥ�� �ٲ�
    projUv.x = input.pPosition.x / input.pPosition.w * 0.5f + 0.5f;
    projUv.y = input.pPosition.y / input.pPosition.w * 0.5f + 0.5f;

    //ȭ�鿡 ���ִ��� �Ǻ�
    //ȭ������� ����� -1~1���� ������ �Ѿ
    if(saturate(projUv.x) == projUv.x&&
       saturate(projUv.y) == projUv.y)
    {
        float4 projMap = NormalMap.Sample(NormalSampler, projUv);
        
        color = projMap;
    }

    return color;
}
