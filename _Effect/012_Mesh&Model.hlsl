#include "000_EffectHeader.hlsl"


// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //
struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 ShadowPos : UV1;
};

VertexOutputDepth VS_Depth(VertexTextureNormalTangent input)
{
    VertexOutputDepth output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, LightView);
    output.Position = mul(output.Position, LightProjection);

    output.Uv = input.Uv;

    return output;
}

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Tangent = WorldTangent(input.Tangent);
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    output.ShadowPos = mul(float4(output.wPosition, 1.0f), ShadowTransform);

    return output;
}

VertexOutput VS_Model(VertexTextureNormalTangent input)
{
    VertexOutput output;

    World = Bones[BoneIndex];
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Tangent = WorldTangent(input.Tangent);
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    output.ShadowPos = mul(input.Position, ShadowTransform);

    return output;
}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
SamplerState Sampler
{
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState CullModeOn
{
    CullMode = None;
};

DepthStencilState DepthOff
{
    DepthEnable = false;
};

float4 BoneColor;
float4 PS_Bone(VertexOutput input) : SV_TARGET
{
    return BoneColor;
}

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normalMap = NormalMap.Sample(Sampler, input.Uv);
    float3 normal;  
    
    float temp = saturate(dot(normalMap, 1));
    [branch]
    if (temp != 0)
        normal=NormalSampleToWorldSpace(normalMap, input.Normal, input.Tangent);
    else
        normal = input.Normal;
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    float shadow = float3(1.0f, 1.0f, 1.0f);
    shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowPos,input.wPosition);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += shadow * D;

    float4 specularMap = SpecularMap.Sample(Sampler, input.Uv);

    temp = saturate(dot(specularMap, 1));

    [branch]
    if (temp != 0)
    {
        specular += shadow * S * specularMap;
    }
    else
    {
        specular += shadow * S;
    }

    float4 diffuseMap = DiffuseMap.Sample(Sampler, input.Uv);

    temp = saturate(dot(diffuseMap, 1));

    [branch]
    if (temp != 0)
    {
        diffuseMap = DiffuseMap.Sample(Sampler, input.Uv);
    }
    else
    {
        diffuseMap = float4(1, 1, 1, 1);
    }

    ambient *= diffuseMap;
    diffuse *= diffuseMap;
    specular *= diffuseMap;

    [unroll]
    for (int i = 0; i < PointLightCount; i++)
    {
        ComputePointLight(m, PointLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    [unroll]
    for (i = 0; i < SpotLightCount; i++)
    {
        ComputeSpotLight(m, SpotLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    float4 color = float4(ambient + diffuse + specular, 1);
    color.a = Diffuse.a;

    return color;
}

void PS_Depth_Alpha(VertexOutputDepth input)
{
    float4 diffuse = DiffuseMap.Sample(samLinear, input.Uv);

    // Don't write transparent pixels to the shadow map.
    clip(diffuse.a - 0.15f);
}

// --------------------------------------------------------------------- //
//  Technique
// --------------------------------------------------------------------- //
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P2
    {
        SetRasterizerState(CullModeOn);
        SetDepthStencilState(DepthOff, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Bone()));
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Depth()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);

        SetRasterizerState(ShadowDepth);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Depth()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Depth_Alpha()));
    }
}