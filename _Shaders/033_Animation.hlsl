#include "000_header.hlsl"

struct PixelInput
{
//	해당 픽셀의 위치
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 ViewDir : VIEWDIR0;
    float3 Tangent : TANGENT0;
};

//	정점 데이터 전달(하나하나)
//	픽셀의 위치 반환
//  함수에 들어온 정점은 WVP변환이 일어남
PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    matrix world = 0;

   [branch]
    if (UseBlend == true)
        world = SkinWorld(input.BlendIndices, input.BlendWeights);
    else
        world = BoneWorld();

    //  행렬 == 공간
    output.Position = mul(input.Position, world); //  웓드 공간
    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

    output.Uv = input.Uv;

    return output;
}


//  Diffuse(DI(Diffuse Intensity * Deffiuse Color)
//  Ambient : 자기 자신의 색
//  Specular : 정반사광, 
//  Diffuse + Ambient + Specular + Emissive(색을 더 부드럽게 강조)
float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    bool isNormal = false;
    bool isSpecular = false;
    
    if (isVisible == 1)
    {
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        if (normal.r!=0)
            isNormal = true;

        if(isNormal)
             NormalMapping(color, normal, input.Normal, input.Tangent);
    }


    float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    if(specular.r!=0)
        isSpecular = true;

    if(isSpecular)
        SpecularLighting(color, specular, input.Normal, input.ViewDir);

    return color;
    //  노말값의 색상 변환
    //  Normal : -1 ~ 1
    //  Normal * 0.5f : -0.5 ~ 0.5
    //  Normal * 0.5f + 0.5f : 0 ~ 1
    //return float4((input.Normal * 0.5f) + 0.5f, 1);

}


//  TODO : float의 오차범위가 왜 생기는지 알아보기
//  TODO : EPSILON 알아보기