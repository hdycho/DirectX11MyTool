#include "000_header.hlsl"

cbuffer VS_Brush : register(b10)
{
    int BrushType;
    float3 Location;
    int Range;
    float3 BrushColor;
}

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor;
    int Spacing; // 한칸의 넓이
    float Thickness;
}

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float3 oPosition : POSITION0;
    float4 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 BrushColor : COLOR0;
};

float3 Brush(float3 location)
{
    if (BrushType == 0)
        return float3(0, 0, 0);
    if (BrushType == 1)
    {
        if ((location.x >= (Location.x - Range)) &&
            (location.x <= (Location.x + Range)) &&
            (location.z >= (Location.z - Range)) &&
            (location.z <= (Location.z + Range)))
        {
            return BrushColor;
        }
    }

    if (BrushType == 2)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if(dist<=Range)
            return BrushColor;
    }
    return float3(0, 0, 0);
}

//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    //  행렬 == 공간
    
    output.oPosition = input.Position;
    
    output.Position = mul(input.Position, World); //  웓드 공간
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영
   
    output.Normal = mul(input.Normal, (float3x3) World);

    output.BrushColor = Brush(input.Position.xyz);

    output.Uv = input.Uv;

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
        if(grid.x<Thickness||grid.y<Thickness)
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

Texture2D ColorMap : register(t10);
SamplerState ColorSampler : register(s10);
//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : 자기 자신의 색
//  Specular : 정반사광, 
//  Diffuse + Ambient + Specular + Emissive(색을 더 부드럽게 강조)
float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 diffuse = ColorMap.Sample(ColorSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    int i = 0;
    for (i = 0; i < SpotLightCount; i++)
        SpotLighting(color, SpotLights[i], input.wPosition, input.Normal);

    for (i = 0; i < AreaLightCount; i++)
        AreaLighting(color, AreaLights[i], input.wPosition, input.Normal);

    //브러쉬는 조명색이 완료된 다음 처리
    color = color + float4(input.BrushColor, 0);
    color = color + float4(Line(input.oPosition),0);

    return color;
}


//  TODO : float의 오차범위가 왜 생기는지 알아보기
//  TODO : EPSILON 알아보기