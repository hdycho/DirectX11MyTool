//  cbuffer 한번 쓰면 안바뀐
cbuffer VS_ViewProjection : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix World;
}

cbuffer VS_Bones : register(b2)
{
    matrix Bones[128];
    matrix BoneScale;

    int UseBlend;
}

cbuffer VS_BoneIndex : register(b3)
{
    int BoneIndex;
}

cbuffer PS_Light : register(b0)
{
    float3 Direction;
    float PS_Light_Padding1;

    float3 Position;
    float PS_Light_Padding2;
}

cbuffer PS_Material : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float SpecularShininess;
}

struct LightingData
{
    float4 LightPositionX;
    float4 LightPositionY;
    float4 LightPositionZ;

    float4 LightDirectionX;
    float4 LightDirectionY;
    float4 LightDirectionZ;

    float4 LightRange;
    
    float4 SpotOuter;
    float4 SpotInner;

    float4 CapsuleLength;

    float4 LightColorR;
    float4 LightColorG;
    float4 LightColorB;
};

cbuffer PS_Lighting : register(b2)
{
    LightingData LightingDatas[6];

    int LightingCount;
}



Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSampler : register(s0);

Texture2D SpecularMap : register(t1);
SamplerState SpecularSampler : register(s1);

Texture2D NormalMap : register(t2);
SamplerState NormalSampler : register(s2);

Texture2D DetailMap : register(t3);
SamplerState DetailSampler : register(s3);

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

///////////////////////////////////////////////////////////////////////////////

matrix BoneWorld()
{
    return mul(Bones[BoneIndex], BoneScale);
}

matrix SkinWorld(float4 indices, float4 weights)
{
    float4x4 transforms = 0;
    transforms += mul(weights.x, Bones[(uint) indices.x]);
    transforms += mul(weights.y, Bones[(uint) indices.y]);
    transforms += mul(weights.z, Bones[(uint) indices.z]);
    transforms += mul(weights.w, Bones[(uint) indices.w]);

    transforms = mul(transforms, BoneScale);
    return transforms;
}

float3 CameraPosition()
{
    return ViewInverse._41_42_43;
}

float3 WorldViewDirection(float4 wPosition)
{
    return normalize(CameraPosition() - wPosition.xyz);
}

float3 WorldNormal(float3 normal)
{
    normal = normalize(mul(normal, (float3x3) World));

    return normal;
}

float3 WorldNormal(float3 normal, matrix world)
{
    normal = normalize(mul(normal, (float3x3) world));

    return normal;
}

float3 WorldTangent(float3 tangent)
{
    tangent = normalize(mul(tangent, (float3x3) World));

    return tangent;
}

float3 WorldTangent(float3 tangent, matrix world)
{
    tangent = normalize(mul(tangent, (float3x3) world));

    return tangent;
}

///////////////////////////////////////////////////////////////////////////////

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float4 SpecularColor;
    float SpecularShininess;
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
    material.SpecularColor = Specular;
    material.SpecularShininess = SpecularShininess;

    return material;
}

float4 dot4x4(float4 x1,float4 y1,float4 z1,float4 x2,float4 y2,float4 z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

float4 dot4x1(float4 x,float4 y,float4 z,float3 b)
{
    return x * b.xxxx + y * b.yyyy + z * b.zzzz;
}

float3 Lighting(LightingData data,float3 wPosition, float3 cPosition, Material material)
{
    float3 eye = normalize(cPosition - wPosition);

    //Capsule Light
    float4 capsuleStartX = wPosition.xxxx - data.LightPositionX;
    float4 capsuleStartY = wPosition.yyyy - data.LightPositionY;
    float4 capsuleStartZ = wPosition.zzzz - data.LightPositionZ;

    float4 distanceOnLine =dot4x4
    (
    capsuleStartX, capsuleStartY, capsuleStartZ,
    data.LightDirectionX, data.LightDirectionY, data.LightDirectionZ
    );
    
    //엡실론 : 1e-6f float이 허용하는 최대 오차범위
    float4 SafeCapsuleLength = max(data.CapsuleLength, 1e-6f);
    distanceOnLine = saturate(distanceOnLine / SafeCapsuleLength) * data.CapsuleLength;

    float4 pointOnLineX = data.LightPositionX + data.LightDirectionX * distanceOnLine;
    float4 pointOnLineY = data.LightPositionY + data.LightDirectionY * distanceOnLine;
    float4 pointOnLineZ = data.LightPositionZ + data.LightDirectionZ * distanceOnLine;
    float4 toLightX = pointOnLineX - wPosition.xxxx;
    float4 toLightY = pointOnLineY - wPosition.yyyy;
    float4 toLightZ = pointOnLineZ - wPosition.zzzz;
    float4 distanceToLightSqrt = dot4x4(toLightX, toLightY, toLightZ, toLightX, toLightY, toLightZ);
    float4 distanceToLight = sqrt(distanceToLightSqrt);


    //Phong Diffuse
    toLightX /= distanceToLight;
    toLightY /= distanceToLight;
    toLightZ /= distanceToLight;

    float4 NdotL = saturate(dot4x1(toLightX, toLightY, toLightZ, material.Normal));
    //float3 color = float3
    //(
    //    dot(data.LightColorR, NdotL),
    //    dot(data.LightColorG, NdotL),
    //    dot(data.LightColorB, NdotL)
    //);

    //blinn Specular
    eye = normalize(eye);
    float4 halfWayX = eye.xxxx + toLightX;
    float4 halfWayY = eye.yyyy + toLightY;
    float4 halfWayZ = eye.zzzz + toLightZ;

    float4 halfWaySize = sqrt(dot4x4(halfWayX, halfWayY, halfWayZ, halfWayX, halfWayY, halfWayZ));
    float4 NdotH = saturate(dot4x1(halfWayX / halfWaySize, halfWayY / halfWaySize, halfWayZ / halfWaySize, material.Normal));
    float4 specular = pow(NdotH, material.SpecularShininess.xxxx) * material.SpecularColor.a;

    //color += float3
    //(
    //    dot(data.LightColorR, specular),
    //    dot(data.LightColorG, specular),
    //    dot(data.LightColorB, specular)
    //);

    //Cone attenuation
    float4 conAngle = dot4x4
    (
        data.LightDirectionX, data.LightDirectionY, data.LightDirectionZ,
        toLightX, toLightY, toLightZ
    );
    float conAttenuation = saturate((conAngle - data.SpotOuter) * data.SpotInner);
    conAttenuation *= conAttenuation;

    //Attenuation
    float4 distanceToLightNormal = 1.0f - saturate(distanceToLight * data.LightRange);
    float4 attenuation = distanceToLightNormal * distanceToLightNormal;
    attenuation *= conAttenuation;

    //최종색
    float4 pixelIntensity = (NdotL + specular) * attenuation;
    float3 color = float3
    (
        dot(data.LightColorR, pixelIntensity),
        dot(data.LightColorG, pixelIntensity),
        dot(data.LightColorB, pixelIntensity)
    );
    color *= material.DiffuseColor;
    
    return color;
}
