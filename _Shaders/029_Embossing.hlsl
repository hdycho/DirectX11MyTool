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

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = float4(0, 0, 0, 1);

    //픽셀아홉개의 마스크값
    float3x3 maskValue =
    {
        { 2, 1, 0 },
        { 1, 0, -1 },
        { 0, -1, -2 },
    };
    float3 grayTone = float3(0.299f, 0.587f, 0.114f);
    float2 uvRatio = float2(1.0f / (float) Width, 1.0f / (float) Height);

    int lineValue = -1;
    int rowValue = -1;

    for (int i = 0; i < 9; i++)
    {
        //픽셀의 9개의 점을 처리하기위해
        float x = uvRatio.x * lineValue;
        float y = uvRatio.y * rowValue;
        float2 uvPosition = float2(input.Uv.x + (float) x, input.Uv.y + (float) y);
        color += dot(Map.Sample(Sampler, uvPosition).rgb, grayTone)*maskValue[rowValue+1][lineValue+1];
       
        lineValue++;
        if ((i + 1) % 3 == 0)
        {
            lineValue = -1;
            rowValue++;
        }
    }
    color.rgb += 0.5;
    color = saturate(color);
    color.a = 1.0f;

    return color;
}
