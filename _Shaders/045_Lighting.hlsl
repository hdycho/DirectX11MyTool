#include "000_header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    matrix world = BoneWorld();
    output.Position = mul(input.Position, world); //  웓드 공간
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영
    
    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    output.cPosition = CameraPosition();
    
    return output;
}

cbuffer PS_Ambient : register(b10)
{
    //  packoffset : 바이트 별로 끊어서 사용

    //  Ambient는 원래 구면 공간으로 만들어야한다.
    //  - 씬에대한 공통적인 색상
    float3 AmbientFloor; //  바닥에서 들어오는 빛 색상
    float PS_Ambient_Padding1;

    float3 AmbientCeil; //  하늘에서 들어오는 빛 색상
    float PS_Ambient_Padding2;

    float4 DirectionColor;
}

cbuffer PS_Specular : register(b11)
{
    float SpecularExp;
    float SpecularIntensity;
}

cbuffer PS_Point : register(b12)
{
    float3 PointLightPosition;
    float PointLightRange;
    float4 PointLightColor;
}

cbuffer PS_Spot : register(b13)
{
    float4 SpotLightColor;

    float3 SpotLightPosition;
    float SpotLightRange;

    float3 SpotLightDirToLight;
    float SpotLightOuter;

    float SpotLightInner;
}

cbuffer PS_Capsule : register(b8)
{
    float4 CapsuleLightColor;

    float3 CapsuleLightPosition;
    float CapsuleLightRange;

    float3 CapsuleLightDirection;
    float CapsuleLightLength;
}

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float SpecularExp;
    float SpecularIntensity;
};

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb;

    //  스페큘러 범위
    //  bling phong
    material.SpecularExp = SpecularExp;
    material.SpecularIntensity = SpecularIntensity;

    return material;
}

float3 CalcAmbient(float3 normal, float3 color)
{
    float up = normal.y * 0.5f + 0.5f;
    float3 ambient = AmbientFloor + up * AmbientCeil;

    return ambient * color;
}

//  TODO : 쿡 토러런스
float3 CalcDirectional(float3 wPosition, float3 cPosition, Material material)
{
    float d = dot(-Direction, material.Normal);
    float3 color = DirectionColor.rgb * saturate(d);

    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + -Direction);
    float h = saturate(dot(halfWay, material.Normal));
    
    color += DirectionColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

    return color * material.DiffuseColor.rgb;
}

float3 CalcPoint(float3 wPosition, float3 cPosition, Material material)
{
    //pointlight
    //광원으로의 방향
    float3 toLight = PointLightPosition.xyz - wPosition;
    //광원과의 거리
    float distanceToLight = length(toLight);
    //방향 정규화
    toLight /= distanceToLight;


    //diffuse
    //빛의 음영(강도)
    float d = dot(toLight, material.Normal);
    float3 color = PointLightColor.rgb * saturate(d);


    //specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal));
    color += PointLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;


    //attenuation(감쇄)
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1 / PointLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // 선형보간
    color *= material.DiffuseColor * attention;

    return color;
}

float3 CalcSpot(float3 wPosition, float3 cPosition, Material material)
{
    //pointlight
    float3 toLight = SpotLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;
    
    //diffuse
    float d = dot(toLight, material.Normal);
    float3 color = SpotLightColor.rgb * saturate(d);
    
    //specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal));
    color += SpotLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

    //spot light
    //광원의 방향과 스팟라이트의 방향의 내적값
    float cosAngle = dot(-SpotLightDirToLight, toLight);

    float outer = cos(radians(SpotLightOuter));
    //이너값은 비율값으로 강도를 결정한다
    float inner = 1.0f/cos(radians(SpotLightInner));

    //outer의 각도는 90도를 넘어가면 안되며(음수값으로 넘어가서)
    //inner또한 90을 넘어가는순간 음수로 값이 떨어지게되어 
    //saturate결과가 0으로 나와 빛이나오지 않는다.
    float conAttention = saturate((cosAngle - outer)*inner);
    conAttention *= conAttention;
    
    //Attenuation
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * (1 / SpotLightRange));
    float attention = distanceToLightNormal * distanceToLightNormal; // 선형보간
    color *= material.DiffuseColor * conAttention * attention;
    return color;
}

float3 CalcCapsule(float3 wPosition, float3 cPosition, Material material)
{
    //CapsuleLight
    float3 dir = normalize(CapsuleLightDirection);
    float3 start = wPosition - CapsuleLightPosition;
    float distanceOnLine = dot(start, dir) / CapsuleLightLength;

    //선분길이보다 큰곳은 잘라내려고 0~1사이로 떨어뜨린다음 다시 선분길이곱함
    distanceOnLine = saturate(distanceOnLine) * CapsuleLightLength;
    
    float3 pointOnLine = CapsuleLightPosition + dir * distanceOnLine;
    float3 toLight = pointOnLine - wPosition;
    float distanceToLight = length(toLight);


    //Diffuse
    toLight /= distanceToLight;
    float d = dot(toLight, material.Normal);
    float3 color = material.DiffuseColor.rgb * saturate(d);

    //Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal));
    color += pow(h, material.SpecularExp) * material.SpecularIntensity;

    //Attenuation
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / CapsuleLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal;
    color *= CapsuleLightColor.rgb * attention;

    return color;
}

///////////////////////////////////////////////////////////////////////////////
////Ambient
///////////////////////////////////////////////////////////////////////////////

//float4 PS(PixelInput input) : SV_TARGET
//{
//    float3 normal = normalize(input.Normal);
//    //  모니터 마다 rgb 색상이 다르기 때문에
//    //  감마색 공간(선형색 공간)을 사용 rgb^2.2
//    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a);    //  a값 : 강도

//    color.rgb = CalcAmbient(normal, color.rgb);
//    color.a = 1.0f;

//    return color;
//}

///////////////////////////////////////////////////////////////////////////////
////Directional
///////////////////////////////////////////////////////////////////////////////

//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);
//    float4 color = 0;
//    color.rgb = CalcAmbient(input.Normal, material.DiffuseColor.rgb);
//    color.rgb *= CalcDirectional(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}

///////////////////////////////////////////////////////////////////////////////
//PointLight
///////////////////////////////////////////////////////////////////////////////

//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);
//    float4 color = 0;
//    color.rgb = CalcAmbient(input.Normal, material.DiffuseColor.rgb);
//    color.rgb *= CalcPoint(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}

///////////////////////////////////////////////////////////////////////////////
//  SpotLight
///////////////////////////////////////////////////////////////////////////////

//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);
//    float4 color = 1;
//    color.rgb *= CalcSpot(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}


float4 PS(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);
    float4 color = 1;
    color.rgb *= CalcCapsule(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}