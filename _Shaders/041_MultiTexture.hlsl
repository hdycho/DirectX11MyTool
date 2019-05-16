#include "000_header.hlsl"

cbuffer PS_Select : register(b10)
{
    int Selected;
}


struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};



PixelInput VS(VertexTexture input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Uv = input.Uv;
    return output;
}

float4 Modulate(float4 arg1,float4 arg2)
{
    return arg1 * arg2;
}

float4 Modulate2X(float4 arg1, float4 arg2)
{
    return arg1 * arg2*2.0f;
}

float4 Modulate4X(float4 arg1, float4 arg2)
{
    return arg1 * arg2 * 4.0f;
}

float4 Add(float4 arg1,float4 arg2)
{
    return arg1 + arg2 ;
}

float4 AddSmooth(float4 arg1, float4 arg2)
{
    return (arg1 + arg2) - (arg1 * arg2);
}

float4 AddSigned(float4 arg1, float4 arg2)
{
    return (arg1 + arg2)-0.5f;
}

float4 AddSigned2X(float4 arg1, float4 arg2)
{
    return ((arg1 + arg2) - 0.5f)*2.0f;
}

float4 Subtract(float4 arg1, float4 arg2)
{
    return arg1-arg2;
}

float4 InvSubtract(float4 arg1, float4 arg2)
{
    return arg2 - arg1;
}

float4 InvSelect1(float4 arg1,float4 arg2)
{
    return 1.0f - arg1;
}

float4 InvSelect2(float4 arg1, float4 arg2)
{
    return 1.0f - arg2;
}

float4 InvAdd(float4 arg1,float4 arg2)
{
    return 1.0f - (arg1 + arg2);
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 arg1 = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 arg2 = NormalMap.Sample(NormalSampler, input.Uv);

    color = arg1;

    [branch]
    switch (Selected)
    {
        case 0:
            color = Modulate(arg1, arg2);
            break;
        case 1:
            color = Modulate2X(arg1, arg2);
            break;
        case 2:
            color = Modulate4X(arg1, arg2);
            break;
        case 3:
            color = Add(arg1, arg2);
            break;
        case 4:
            color = AddSmooth(arg1, arg2);
            break;
        case 5:
            color = AddSigned(arg1, arg2);
            break;
        case 6:
            color = AddSigned2X(arg1, arg2);
            break;
        case 7:
            color = Subtract(arg1, arg2);
            break;
        case 8:
            color = InvSubtract(arg1, arg2);
            break;
        case 9:
            color = InvSelect1(arg1, arg2);
            break;
        case 10:
            color = InvSelect2(arg1, arg2);
            break;
        case 11:
            color = InvAdd(arg1, arg2);
            break;
    }

    return float4(color.rgb, 1);
}
