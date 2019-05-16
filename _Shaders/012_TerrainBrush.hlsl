#include "000_header.hlsl"

cbuffer VS_Brush : register(b10)
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
    float3 BrushColor : COLOR0;
};

float3 BrushColor(float3 location)
{
    //location은 버텍스위치
    //Location은 지형의 피킹 위치
    if (Type==0)
        return float3(0, 0, 0);
    //사각형일때
    if(Type==1)
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
    if(Type==2)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if(dist<=Range)
            return Color;
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
    output.Position = mul(input.Position, World); //  웓드 공간
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = mul(input.Normal, (float3x3) World);

    output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    //브러쉬는 조명색이 완료된 다음 처리
    //브러쉬컬러는 float3이기 때문에 float4로 형변환하고 
    //알파값은 의미가 없기 때문에 0으로 둔다.
    color = color + float4(input.BrushColor, 0);

    return color;
}


//  TODO : float의 오차범위가 왜 생기는지 알아보기
//  TODO : EPSILON 알아보기