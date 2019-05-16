struct Bone
{
    matrix BindPose;
};
StructuredBuffer<Bone> BoneBuffer;

ByteAddressBuffer Vertex;
RWByteAddressBuffer Result;

struct VertexTextureNormalTangentBlend
{
    float4 Position;
    float2 Uv;
    float3 Normal;
    float3 Tangent;
    float4 BlendIndices;
    float4 BlendWeights;
};

void Skinning(inout VertexTextureNormalTangentBlend vertex)
{
    float4x4 m = 0;
    float3x3 m3 = 0;

    float weightSum = 0.0f;
    float4 position = 0;
    float3 normal = 0, tangent = 0;

    [unroll]
    for (uint i = 0; ((i < 4) && (weightSum < 1.0f)); i++)
    {
        m = BoneBuffer[(uint) vertex.BlendIndices[i]].BindPose;
        m3 = (float3x3) m;

        position += mul(vertex.Position, m) * vertex.BlendWeights[i];
        normal += mul(vertex.Normal, m3) * vertex.BlendWeights[i];
        tangent += mul(vertex.Tangent, m3) * vertex.BlendWeights[i];

        weightSum += vertex.BlendWeights[i];
    }

    bool w = any(weightSum);
    vertex.Position = w ? position : vertex.Position;
    vertex.Normal = w ? normal : vertex.Normal;
    vertex.Tangent = w ? tangent : vertex.Tangent;

    //vertex.Position = vertex.Position;
    //vertex.Normal = vertex.Normal;
    //vertex.Tangent =vertex.Tangent;
}

[numthreads(256, 1, 1)] //  스레드 그룹 스레드 수
void CS(uint3 dispatchId : SV_DispatchThreadID)
{
    int fetchAddress = dispatchId.x * 20 * 4;
    
    //Vertex Texture Normal Tangent BlendIndices BlendWeight
    //  4      2       3      3         4            4
    //  0      16      24     36        48           64        80

    VertexTextureNormalTangentBlend vertex;

    vertex.Position     = asfloat(Vertex.Load4(fetchAddress + 0));
    vertex.Uv           = asfloat(Vertex.Load2(fetchAddress + 16));
    vertex.Normal       = asfloat(Vertex.Load3(fetchAddress + 24));
    vertex.Tangent      = asfloat(Vertex.Load3(fetchAddress + 36));
    vertex.BlendIndices = asfloat(Vertex.Load4(fetchAddress + 48));
    vertex.BlendWeights = asfloat(Vertex.Load4(fetchAddress + 64));

    Skinning(vertex);

    Result.Store4(fetchAddress + 0, asuint(vertex.Position));
    Result.Store2(fetchAddress + 16, asuint(vertex.Uv));
    Result.Store3(fetchAddress + 24, asuint(vertex.Normal));
    Result.Store3(fetchAddress + 36, asuint(vertex.Tangent));
    Result.Store4(fetchAddress + 48, asuint(vertex.BlendIndices));
    Result.Store4(fetchAddress + 64, asuint(vertex.BlendWeights));

}


// --------------------------------------------------------------------- //
//  Technique
// --------------------------------------------------------------------- //
technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }

}