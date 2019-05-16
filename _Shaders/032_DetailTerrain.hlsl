#include "000_header.hlsl"

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor;
    int Spacing; // ��ĭ�� ����
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
//	�ش� �ȼ��� ��ġ
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
    //location�� ���ؽ���ġ
    //Location�� ������ ��ŷ ��ġ
    if (Type == 0)
        return float3(0, 0, 0);
    //�簢���϶�
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
    //���϶�
    if (Type == 2)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if (dist <= Range)
            return Color;
    }

     //���϶�
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

    //  ��� == ����
    output.oPosition = input.Position;

    output.Position = mul(input.Position, World); //  �d�� ����
    output.Position = mul(output.Position, View); //  ���� ��ġ�� ȸ�� ��
    output.Position = mul(output.Position, Projection); //  ����

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
    [branch] // �Ѵٽ����ϰ� �ϳ��� ������ if else if��밡��
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



