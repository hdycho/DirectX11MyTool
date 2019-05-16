//=====================================================//
// CBuffer
//=====================================================//
cbuffer CB_VP
{
    matrix View;
    matrix Projection;
};

cbuffer CB_World
{
    matrix World;
};

cbuffer CB_Light
{
    float3 Direction;
    float3 Position;
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