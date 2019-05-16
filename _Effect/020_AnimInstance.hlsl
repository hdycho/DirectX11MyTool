#include "000_EffectHeader.hlsl"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

Texture2DArray Transforms;
float4 ClipPlane[6];

struct RenderDesc
{
    float isRender;
    float3 padding;
};
RenderDesc renderState[64];

struct FrameDesc
{
    uint Curr;
    uint Next;
    float Time;
    float FrameTime;
    float BlendTime;
    uint playClipIdx;
    uint nextClipIdx;
    float padding;

    float4 diffuse;
};
FrameDesc Frames[64];

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

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;

    //TODO: fixed
    matrix Transform : Instance0;
    uint InstID : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position : SV_POSITION0; // 0밖에 사용 불가함
    float3 wPosition : Position1;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;

    float4 ShadowPos : TEXCOORD1;
    float4 Clip : SV_CullDistance0; // VS에서 버텍스컬링하는 방법=>프러

    float4 DiffuseColor : Diffuse0;
};

struct VertexOutputDepthClip
{
    float4 Position : SV_Position0;
    float3 wPosition : Position1;
    float2 Uv : Uv0;

    float4 Clip : SV_CullDistance0; // VS에서 버텍스컬링하는 방법=>프러
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    float boneIndices[4] =
    {
        input.BlendIndices.x,
        input.BlendIndices.y,
        input.BlendIndices.z,
        input.BlendIndices.w,
    };

    float boneWeights[4] =
    {
        input.BlendWeights.x,
        input.BlendWeights.y,
        input.BlendWeights.z,
        input.BlendWeights.w,
    };

    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;
    matrix anim = 0;

    [flatten]
    if (Frames[input.InstID].playClipIdx == 0)  //  T포즈일때
    {
        float4 m0 = Transforms.Load(int4(BoneIndex * 4 + 0, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m1 = Transforms.Load(int4(BoneIndex * 4 + 1, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m2 = Transforms.Load(int4(BoneIndex * 4 + 2, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m3 = Transforms.Load(int4(BoneIndex * 4 + 3, input.InstID, Frames[input.InstID].playClipIdx, 0));

        transform = matrix(m0, m1, m2, m3);
    }
    else
    {
        [flatten]
        if (Frames[input.InstID].nextClipIdx == 0) // 블랜드처리안할때
        {
            transform = 0;
            [unroll]
            for (int i = 0; i < 4; i++)
            {
                c0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                curr = matrix(c0, c1, c2, c3);

                n0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                next = matrix(n0, n1, n2, n3);

                anim = lerp(curr, next, (matrix) Frames[input.InstID].Time);
        
                transform += mul(boneWeights[i], anim);
                
            }
        }
        else // 블랜드처리할때
        {
            transform = 0;
            [unroll]
            for (int i = 0; i < 4; i++)
            {
                c0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                curr = matrix(c0, c1, c2, c3);

                n0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                next = matrix(n0, n1, n2, n3);

                anim = lerp(curr, next, (matrix) Frames[input.InstID].BlendTime);
        
                transform += mul(boneWeights[i], anim);
            }
        }
    }
    World = mul(transform, input.Transform);

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.DiffuseColor = Frames[input.InstID].diffuse;

    output.ShadowPos = mul(float4(output.wPosition, 1.0f), ShadowTransform);
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

VertexOutputDepthClip VS_Depth(VertexInput input)
{
    VertexOutputDepthClip output;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    float boneIndices[4] =
    {
        input.BlendIndices.x,
        input.BlendIndices.y,
        input.BlendIndices.z,
        input.BlendIndices.w,
    };

    float boneWeights[4] =
    {
        input.BlendWeights.x,
        input.BlendWeights.y,
        input.BlendWeights.z,
        input.BlendWeights.w,
    };

    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;
    matrix anim = 0;

    [flatten]
    if (Frames[input.InstID].playClipIdx == 0)  //  T포즈일때
    {
        float4 m0 = Transforms.Load(int4(BoneIndex * 4 + 0, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m1 = Transforms.Load(int4(BoneIndex * 4 + 1, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m2 = Transforms.Load(int4(BoneIndex * 4 + 2, input.InstID, Frames[input.InstID].playClipIdx, 0));
        float4 m3 = Transforms.Load(int4(BoneIndex * 4 + 3, input.InstID, Frames[input.InstID].playClipIdx, 0));

        transform = matrix(m0, m1, m2, m3);
    }
    else
    {
        [flatten]
        if (Frames[input.InstID].nextClipIdx == 0) // 블랜드처리안할때
        {
            transform = 0;
            [unroll]
            for (int i = 0; i < 4; i++)
            {
                c0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                curr = matrix(c0, c1, c2, c3);

                n0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                n3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Next, Frames[input.InstID].playClipIdx, 0));
                next = matrix(n0, n1, n2, n3);

                anim = lerp(curr, next, (matrix) Frames[input.InstID].Time);
        
                transform += mul(boneWeights[i], anim);
            }
        }
        else // 블랜드처리할때
        {
            transform = 0;
            [unroll]
            for (int i = 0; i < 4; i++)
            {
                c0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                c3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Curr, Frames[input.InstID].playClipIdx, 0));
                curr = matrix(c0, c1, c2, c3);

                n0 = Transforms.Load(uint4(boneIndices[i] * 4 + 0, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n1 = Transforms.Load(uint4(boneIndices[i] * 4 + 1, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n2 = Transforms.Load(uint4(boneIndices[i] * 4 + 2, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                n3 = Transforms.Load(uint4(boneIndices[i] * 4 + 3, Frames[input.InstID].Next, Frames[input.InstID].nextClipIdx, 0));
                next = matrix(n0, n1, n2, n3);

                anim = lerp(curr, next, (matrix) Frames[input.InstID].BlendTime);
        
                transform += mul(boneWeights[i], anim);
            }
        }
    }
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

    Material m = { Ambient, input.DiffuseColor, Specular, Shininess };
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

    if (input.DiffuseColor.r == 1 && input.DiffuseColor.g == 0 && input.DiffuseColor.b==0)
    {
        color = input.DiffuseColor;
        color *= diffuseMap;

        color.a = Diffuse.a;
    }
    else
    {
        temp = saturate(dot(diffuseMap, 1));

        if (temp > 0.0f)
            color *= diffuseMap;

        color.a = Diffuse.a;
    }

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

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
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