#include "000_EffectHeader.hlsl"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

Texture2D Transforms;

struct RenderDesc
{
    float isRender;
    float3 padding;
};

RenderDesc renderState[64];

float4 ClipPlane[6];

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;

    matrix Transform : Instance0;
    uint InstID : SV_InstanceID;
};

struct VertexOutputDepthClip
{
    float4 Position : SV_Position0;
    float3 wPosition : Position1;
    float2 Uv : Uv0;

    float4 Clip : SV_CullDistance0; // VS에서 버텍스컬링하는 방법=>프러
};

bool AabbOutsideFrustumTest(float3 center, float3 extents)
{
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
      // If the box is completely behind any of the frustum planes
      // then it is outside the frustum.
        [flatten]
        if (AabbBehindPlaneTest(center, extents, ClipPlane[i]))
        {
            return true;
        }
    }
    return false;
}

struct VertexOutput
{
    float4 Position : SV_POSITION0; // 0밖에 사용 불가함
    float3 wPosition : Position1;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;

    float4 ShadowPos : TEXCOORD1;
    float4 Clip : SV_CullDistance0; // VS에서 버텍스컬링하는 방법=>프러
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float4 m0 = Transforms.Load(int3(BoneIndex * 4 + 0, input.InstID, 0));
    float4 m1 = Transforms.Load(int3(BoneIndex * 4 + 1, input.InstID, 0));
    float4 m2 = Transforms.Load(int3(BoneIndex * 4 + 2, input.InstID, 0));
    float4 m3 = Transforms.Load(int3(BoneIndex * 4 + 3, input.InstID, 0));

    matrix transform = matrix(m0, m1, m2, m3);
    World = mul(transform, input.Transform);

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);

    output.ShadowPos = mul(float4(output.wPosition, 1.0f), ShadowTransform);
    output.Tangent = WorldTangent(input.Tangent);

    output.Uv = input.Uv;

    [unroll]
    for (int i = 0; i < 6; i++)
    {
        if (AabbOutsideFrustumTest(output.wPosition,0))
        {
            output.Clip = -1;
        }
        else
            output.Clip = 1;
    }

    if (renderState[input.InstID].isRender == false)
        output.Clip = -1;

    return output;
}

VertexOutputDepthClip VS_Depth(VertexInput input)
{
    VertexOutputDepthClip output;

    float4 m0 = Transforms.Load(int3(BoneIndex * 4 + 0, input.InstID, 0));
    float4 m1 = Transforms.Load(int3(BoneIndex * 4 + 1, input.InstID, 0));
    float4 m2 = Transforms.Load(int3(BoneIndex * 4 + 2, input.InstID, 0));
    float4 m3 = Transforms.Load(int3(BoneIndex * 4 + 3, input.InstID, 0));

    matrix transform = matrix(m0, m1, m2, m3);
    World = mul(transform, input.Transform);

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;
    output.Position = mul(output.Position, LightView);
    output.Position = mul(output.Position, LightProjection);

    output.Uv = input.Uv;

    [unroll]
    for (int i = 0; i < 6; i++)
    {
        if (AabbOutsideFrustumTest(output.wPosition, 0))
        {
            output.Clip = -1;
        }
        else
            output.Clip = 1;
    }

    if (renderState[input.InstID].isRender == false)
        output.Clip = -1;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normalMap = NormalMap.Sample(Sampler, input.Uv);
    
    float3 normal = 0;

    float temp = saturate(dot(normalMap, 1));

    [flatten]
    if (temp > 0.0f)
        normal = NormalSampleToWorldSpace(normalMap, input.Normal, input.Tangent);
    else
        normal = input.Normal;
    
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    float shadow = float3(1.0f, 1.0f, 1.0f);
    shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowPos, input.wPosition);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { { LightAmbient.r + 0.3, LightAmbient.g + 0.3, LightAmbient.b + 0.3, LightAmbient.a }, LightDiffuse, LightSpecular, LightDirection };

    float4 specularMap = SpecularMap.Sample(Sampler, input.Uv);

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A * (shadow + 0.5f);
    diffuse += D * (shadow + 0.5f);
    specular += S * (shadow + 0.5f);
    

    temp = saturate(dot(specularMap, 1));
    if (temp > 0.0f)
        specular *= specularMap;
       
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


    float4 diffuseMap = DiffuseMap.Sample(Sampler, input.Uv);

    float rimLight;
    float4 t = color;
    
    rimLight = RimLighting(normal, toEye);
    t *= rimLight;
    color += t;

    temp = saturate(dot(diffuseMap, 1));

    if (temp > 0.0f)
        color *= diffuseMap;

    color.a = Diffuse.a;

    return color;
}

void PS_Depth_Alpha(VertexOutputDepthClip input)
{
    float4 diffuse = DiffuseMap.Sample(samLinear, input.Uv);

    // Don't write transparent pixels to the shadow map.
    clip(diffuse.a - 0.15f);
}
//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------
BlendState BlendOn
{
    BlendEnable[0] = true;
    BlendOp[0] = Add;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = Inv_Src_Alpha;
    DestBlendAlpha[0] = Zero;
    SrcBlend[0] = Src_Alpha;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
        //SetGeometryShader(CompileShader(gs_5_0, GS()));

        SetBlendState(BlendOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Depth()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Depth_Alpha()));

        SetRasterizerState(ShadowDepth);
    }
}