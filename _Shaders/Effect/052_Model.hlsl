#include "000_EffectHeader.hlsl"

//-----------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------
struct VertexOuput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOuput VS_Bone(VertexTextureNormalTangentBlend input)
{
    VertexOuput output;

    World = Bones[BoneIndex];
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

VertexOuput VS_Animation(VertexTextureNormalTangentBlend input)
{
    VertexOuput output;

    World = SkinWorld(input.BlendIndices, input.BlendWeights);
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOuput input) : SV_TARGET
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);
    
    float3 normal = normalize(input.Normal);
    float NdotL = dot(-LightDirection, normal);
    
    return diffuse * NdotL;

    //float4 diffuseColor = DiffuseMap.Sample(Sampler, input.Uv);
    //float4 color = diffuseColor;

    //float3 normal = normalize(input.Normal);
    //float NdotL = dot(-LightDirection, normal);

    //color *= NdotL;
   

    //float4 specularColor = SpecularMap.Sample(Sampler, input.Uv);

    //float4 normalMap = NormalMap.Sample(Sampler, input.Uv);

    //if (normalMap.r != 0)
    //{
    //    NormalMapping(color, normalMap, input.Normal, input.Tangent);
    //}

    //if (specularColor.r != 0)
    //{
    //    SpecularLighting(color, specularColor, input.Normal, -ViewDirection);
    //}
}

//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Bone()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Animation()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}