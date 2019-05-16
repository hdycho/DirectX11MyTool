#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Width;
    int Height;
    int Count;
}


SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);


    //NDC 공간 - FVF_RHWXYZ
    //output.Position = input.Position;
    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}

static const int BlurCount = 20;
static const float Weights[13] =
{
    0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f
    , 1.0f,
    0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};
static const float sumWeights = 6.2108f;

float4 PS_X(PixelInput input):SV_TARGET
{
    float2 uv = input.Uv;
    float u = 1.0f / (float)Width;
    
    float sum = 0;
    float4 diffuse = 0;
    for (int i = -Count; i <= Count; i++)
    {
        float2 temp = uv + float2(u * (float) i, 0);
        
        diffuse += Weights[6 + i] * Map.Sample(Sampler, temp);
        sum += Weights[6 + i];
    }
    diffuse /= sum;

    return float4(diffuse.rgb, 1);
}

float4 PS_Y(PixelInput input) : SV_TARGET
{
    float2 uv = input.Uv;
    float v = 1.0f / (float) Height;
    
    float sum = 0;
    float4 diffuse = 0;
    for (int i = -Count; i <= Count; i++)
    {
        float2 temp = uv + float2(0, v * (float) i);
        
        diffuse += Weights[6 + i] * Map.Sample(Sampler, temp);
        sum += Weights[6 + i];
    }
    diffuse /= sum;

    return float4(diffuse.rgb, 1);
}

float4 PS_XY(PixelInput input) : SV_TARGET
{
    return Map.Sample(Sampler, input.Uv);
}

