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
    int Spacing; // ��ĭ�� ����
    float Thickness;
}

struct PixelInput
{
//	�ش� �ȼ��� ��ġ
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

//	���� ������ ����(�ϳ��ϳ�)
//	�ȼ��� ��ġ ��ȯ
//  �Լ��� ���� ������ WVP��ȯ�� �Ͼ
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    //  ��� == ����
    
    output.oPosition = input.Position;
    
    output.Position = mul(input.Position, World); //  �d�� ����
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����
   
    output.Normal = mul(input.Normal, (float3x3) World);

    output.BrushColor = Brush(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

float3 Line(float3 oPosition)
{
    //Line Drawing
    [branch] // �Ѵٽ����ϰ� �ϳ��� ������ if else if��밡��
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

        //�������� ���ߴ��۾�(��ĭ�� ����)
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        
        //��̺�(fwidth) 
        //����󿡼� xz�� ��ũ���󿡼� xy�� ��Ÿ�����°Ϳ�
        //���⸦ ���ϱ����� ��̺� ���
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
//  Ambient : �ڱ� �ڽ��� ��
//  Specular : ���ݻ籤, 
//  Diffuse + Ambient + Specular + Emissive(���� �� �ε巴�� ����)
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

    //�귯���� ������� �Ϸ�� ���� ó��
    color = color + float4(input.BrushColor, 0);
    color = color + float4(Line(input.oPosition),0);

    return color;
}


//  TODO : float�� ���������� �� ������� �˾ƺ���
//  TODO : EPSILON �˾ƺ���