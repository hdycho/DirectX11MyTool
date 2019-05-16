#include "001_Header2.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  �d�� ����
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����
    
    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    output.cPosition = CameraPosition();
    
    return output;
}


//float3 CalcAmbient(float3 normal, float3 color)
//{
//    float up = normal.y * 0.5f + 0.5f;
//    float3 ambient = AmbientFloor + up * AmbientCeil;

//    return ambient * color;
//}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    //  ����� ���� rgb ������ �ٸ��� ������
    //  ������ ����(������ ����)�� ��� rgb^2.2
    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); //  a�� : ����

    Material material = CreateMaterial(input.Normal, input.Uv);
    color.rgb = Lighting(LightingDatas[0], input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;
    
    return color;
}