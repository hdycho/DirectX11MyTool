//=====================================================//
// CBuffer
//=====================================================//
cbuffer CB_PerFrame
{
    matrix View;
    float3 ViewDirection;

    float Time;
    float3 ViewPosition;
};

cbuffer CB_Projection
{
    matrix Projection;
};

cbuffer CB_World
{
    matrix World;
};

cbuffer CB_Light
{
    float4 SunColor;
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 LightDirection;
}

cbuffer CB_Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float Shininess;
}


//=====================================================//
// Textures
//=====================================================//
Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D DetailMap;


//=====================================================//
// VertexLayout
//=====================================================//
struct Vertex
{
    float4 Position : POSITION0;
};

struct VertexSize
{
    float4 Position : POSITION0;
    float2 Size : SIZE0;
};

struct VertexColor
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormalTangent
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

struct VertexTextureNormalTangentBlend
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;
};

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};


//=====================================================//
// Bones
//=====================================================//
cbuffer CB_Bones
{
    matrix Bones[128];
};
int BoneIndex;

matrix BoneWorld()
{
    return Bones[BoneIndex];
}

matrix SkinWorld(float4 indices, float4 weights)
{
    float4x4 transforms = 0;
    transforms += mul(weights.x, Bones[(uint) indices.x]);
    transforms += mul(weights.y, Bones[(uint) indices.y]);
    transforms += mul(weights.z, Bones[(uint) indices.z]);
    transforms += mul(weights.w, Bones[(uint) indices.w]);

    transforms = transforms;
    return transforms;
}

//-----------------------------------------------------------------------------
// Shadow
//-----------------------------------------------------------------------------
matrix ShadowTransform;
matrix LightView;
matrix LightProjection;

struct VertexOutputDepth
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
};

RasterizerState ShadowDepth
{
    DepthBias = 10000;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.0f;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D ShadowMap;
SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2D shadowMap,
                       float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
    
    // Depth in NDC space.
    float depth = shadowPosH.z - 0.0005f;

    // Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return saturate(percentLit /= 9.0f);
}


//-----------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------
float3 WorldNormal(float3 normal)
{
    return normalize(mul(normal, (float3x3) World));
}

float3 WorldTangent(float3 tangent)
{
    return normalize(mul(tangent, (float3x3) World));
}

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);
   
   // This is always positive.
    float r = dot(extents, n);
   
   // signed distance from center point to plane.
    float s = dot(float4(center, 1.0f), plane);
   
   // If the center point of the box is a distance of e or more behind the
   // plane (in which case s is negative since it is behind the plane),
   // then the box is completely in the negative half space of the plane.
    return (s + r) < 0.0f;
}


//-----------------------------------------------------------------------------
// Lighting
//-----------------------------------------------------------------------------
void DiffuseLighting(inout float4 color, float4 diffuse, float3 normal)
{
    float intensity = saturate(dot(normal, -LightDirection));

    color = color + Diffuse*diffuse * intensity;
    //color = color +diffuse * intensity;
}

void SpecularLighting(inout float4 color, float4 specularMap, float3 normal, float3 viewDirection)
{
    float3 reflection = reflect(LightDirection, normal);
    float intensity = saturate(dot(reflection, viewDirection));
    float specular = pow(intensity, 1);

    color = color +Specular*specular * specularMap;
    //color = color +specular * specularMap;
}

void NormalMapping(inout float4 color, float4 normalMap, float3 normal, float3 tangent)
{
    float3 N = normal; //Z축
    float3 T = normalize(tangent - dot(tangent, N) * N); //X축
    float3 B = cross(T, N); //Y축
    
    //탄젠트공간 생성
    float3x3 TBN = float3x3(T, B, N);

    float3 coord = 2.0f * normalMap - 1.0f;
    float3 bump = mul(coord, TBN);

    //음영식

    float3 lightDir = LightDirection;
    
    if(lightDir.y>=0.0f)
        lightDir.y = lightDir.y * -1.0f;

    float intensity = saturate(dot(bump, -lightDir));
    
    color = color * intensity;
}

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float Shininess;
};

//-----------------------------------------------------------------------------
// Directional Lighting
//-----------------------------------------------------------------------------
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
};

void ComputeDirectionalLight(Material m, DirectionalLight l, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float temp = 0.0f;
    float3 light = -l.Direction;
    
    // 밤이됬을때는 방향을다시 뒤집어서 처리한다
    if (l.Direction.y >= 0.0f)
    {
        //temp = 0.2f;
        //if (l.Direction.y <= 0.3f)
        //{
            temp = smoothstep(0.0f, 0.3f, l.Direction.y / 0.3f);
            temp *= 0.2f;
        //}

        light = light * -1.0f;
    }

    float3 nightLightColor = (SunColor.rgb + temp);
  
    
    light *= saturate(nightLightColor);

    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        
        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }
}


//-----------------------------------------------------------------------------
// Point Lighting
//-----------------------------------------------------------------------------
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Attenuation;
    float PointLight_Padding;
};

cbuffer CB_PointLight
{
    PointLight PointLights[16];
    int PointLightCount;
};

void ComputePointLight(Material m, PointLight l, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float3 light = l.Position - position;
    float dist = length(light);
    
    if (dist > l.Range)
        return;

    light /= dist;
    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }


    float attenuate = 1.0f / dot(l.Attenuation, float3(1.0f, dist, dist * dist));

    diffuse *= attenuate;
    specular *= attenuate;
}

//-----------------------------------------------------------------------------
// Spot Lighting
//-----------------------------------------------------------------------------
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float PointLight_Padding;

    float3 Direction;
    float Spot;

    float3 Attenuation;
    float PointLight_Padding2;
};

cbuffer CB_SpotLight
{
    SpotLight SpotLights[16];
    int SpotLightCount;
};

void ComputeSpotLight(Material m, SpotLight l, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float3 light = l.Position - position;
    float dist = length(light);

    light /= dist;
    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }


    float spot = pow(max(dot(-light, l.Direction), 0.0f), l.Spot);
    float attenuate = spot / dot(l.Attenuation, float3(1.0f, dist, dist * dist));

    ambient *= attenuate;
    diffuse *= attenuate;
    specular *= attenuate;
}

//---------------------------------------------------------------------------------------
// Normal Mapping
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMap, float3 normal, float3 tangent)
{
	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMap - 1.0f;

	// Build orthonormal basis.
    float3 N = normal;
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}