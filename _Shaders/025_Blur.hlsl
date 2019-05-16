#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
};

////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Select;
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

    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}

static const int BlurCount = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float x = input.Uv.x;
    float y = input.Uv.y;

    float4 color = Map.Sample(Sampler, float2(x, y));

    float2 temp = 0;
    float2 arr[BlurCount];
    for (int i = 0; i < Count; i++)
    {
        arr[i] = float2(0, 0);
        arr[i].x = (i + 1) / (float) Width;
        arr[i].y = (i + 1) / (float) Height;

        temp = float2(x - arr[i].x, y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x + arr[i].x, y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x, y-arr[i].y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x, y+arr[i].y);
        color += Map.Sample(Sampler, temp);
    }
    
    color /= (float) Count * 4.0f + 1; // 자기자신의 갯수도 1더해줘야해서
    color.a = 1;
    return color;
}

//float4 PS2(PixelInput input) : SV_TARGET
//{
//    float x = input.Uv.x;
//    float y = input.Uv.y;

//    float4 color = Map.Sample(Sampler, float2(x, y));

//    float ratioX = x / (float) Width;
//    float ratioY = y / (float) Height;

//    float2 left = float2(x-ratioX, y);
//    float2 right = float2(x + ratioX, y);
//    float2 upper = float2(x, y - ratioY);
//    float2 lower = float2(x, y + ratioY);

//    color += Map.Sample(Sampler, left);
//    color += Map.Sample(Sampler, right);
//    color += Map.Sample(Sampler, upper);
//    color += Map.Sample(Sampler, lower);

//    color.rgb /= 5;

//    color.a = 1.0f;
//    return color;
//}