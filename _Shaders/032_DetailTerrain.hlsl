#include "000_header.hlsl"

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor;
    int Spacing; // 한칸의 넓이
    float Thickness;
}

cbuffer PS_DetailSet : register(b11)
{
    float DetailLightPower;
    float2 Depth;
};

cbuffer VS_DetailSet : register(b12)
{
    int UvSize;
    float2 VertexDepth;
};

cbuffer VS_Brush : register(b13)
{
    int Type;
    float3 Location;
    int Range;
    float3 Color;
}

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float3 oPosition : OPOSITION0;
    float4 pPosition : PPOSITION0;
    float3 View : VIEW0;
    float3 BrushColor: COLOR0;
};

float3 BrushColor(float3 location)
{
    //location은 버텍스위치
    //Location은 지형의 피킹 위치
    if (Type == 0)
        return float3(0, 0, 0);
    //사각형일때
    if (Type == 1)
    {
        if ((location.x >= (Location.x - Range)) &&
            (location.x <= (Location.x + Range)) &&
            (location.z >= (Location.z - Range)) &&
            (location.z <= (Location.z + Range)))
        {
            return Color;
        }
    }
    //원일때
    if (Type == 2)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if (dist <= Range)
            return Color;
    }

     //원일때
    if (Type == 3)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if (dist <= Range)
            return Color;
    }

    return float3(0, 0, 0);
}

PixelInput VS(VertexTextureNormalTangent input)
{
    PixelInput output;

    //  행렬 == 공간
    output.oPosition = input.Position;

    output.Position = mul(input.Position, World); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = WorldNormal(input.Normal, World);
    output.Tangent = WorldTangent(input.Tangent, World);

    output.pPosition = output.Position;

    output.View = ViewInverse._41_42_43 - output.pPosition.xyz;

    output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv * UvSize;

    return output;
}

float3 Line(float3 oPosition)
{
    //Line Drawing
    [branch] // 둘다실행하고 하나만 선택함 if else if사용가능
    if (LineType == 1)
    {
        float2 grid = float2(0, 0);
        grid.x = frac(oPosition.x / (float) Spacing);
        grid.y = frac(oPosition.z / (float) Spacing);

        [flatten]
        if (grid.x < Thickness || grid.y < Thickness)
            return LineColor;
    }
    else if (LineType == 2)
    {
        float2 grid = oPosition.xz / (float) Spacing;

        //끝지점을 맞추는작업(한칸의 범위)
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        
        //편미분(fwidth) 
        //월드상에선 xz가 스크린상에선 xy로 나타내지는것에
        //기울기를 구하기위해 편미분 사용
        float2 speed = fwidth(range);

        float2 pixel = range / speed;
        float weights = saturate(min(pixel.x, pixel.y) - Thickness);

        return lerp(LineColor, float3(0, 0, 0), weights);
    }

    return float3(0, 0, 0);
}

float4 PS(PixelInput input) : SV_TARGET
{
    float depth = input.pPosition.z/input.pPosition.w;
    
    float4 color = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    
    [branch]
    if (depth < Depth.x) // 0.99
    {
        float4 nomalMap = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, nomalMap, input.Normal, input.Tangent);
        
        float4 detailMap = DetailMap.Sample(DetailSampler, input.Uv);
        color = color * detailMap * DetailLightPower;
    }
    else if (depth > Depth.y) //0.995
    {
        color = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    }
    else if (depth > Depth.x)
    {
        float4 nomalMap = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, nomalMap, input.Normal, input.Tangent);
    }

    float4 result = 0;

    DiffuseLighting(result, color, input.Normal);
    //SpecularLighting(result, input.Normal, input.View);

    result = result + float4(input.BrushColor, 0);
    result = result + float4(Line(input.oPosition), 0);

    return result;
}



