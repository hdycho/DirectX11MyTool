#include "000_Header.hlsl"

struct PixelInput
{
                    //sementic
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

Texture2D Map2 : register(t1);
Texture2D Map3 : register(t2);

//정점데이터 전달(하나하나)
//픽셀의 위치반환
//함수에 들어온정점은 wvp변환 일어남
PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = mul(input.Normal, (float3x3)World);
    
    output.Uv = input.Uv;
    
    return output; // 반환값이 픽셀의 위치
}

//Ambient : 자기자신의색
//Specular : 정반사광
//Diffuse + Ambient + Emissive(색을좀더부드럽게)

float4 PS(PixelInput input) : SV_TARGET
{
    //saturate : 0~1로 값제한
    float4 t = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 t2 = Map2.Sample(DiffuseSampler, input.Uv);
    float4 alpha = Map3.Sample(DiffuseSampler, input.Uv);

    float4 diffuse = (1.0f - alpha.r) * t + t2 * alpha.r;

    float4 color = 0;
    DiffuseLight(color,diffuse, input.Normal);

    
    return color;
    //return float4((input.normal * 0.5f) + 0.5f,1); //노말벡터를 색상으로 쉐이더디버깅용으로 사용할수있음 
}