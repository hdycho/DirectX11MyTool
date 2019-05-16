#include "000_EffectHeader.hlsl"


struct VertexOutput
{
    float4 Position : SV_POSITION0;


    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_Model(VertexTextureNormalTangent input)
{
    VertexOutput output;

    World = Bones[BoneIndex];
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = mul(input.Tangent, (float3x3) World);
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////


SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube CubeMap;

float n1 = 1.0;
float n2 = 1.8f;

//스넬의 법칙
//n1*sin0 = n2* sin0

float3 GetNormal(float3 normal, float2 uv, Texture2D map, SamplerState state, uniform int power = 1)
{
    float3 B = { 1, 0, 0 };
    float3 T = { 0, 1, 0 };
    float3 N = { 0, 0, 1 };

    N = normalize(normal);
    T = normalize(cross(N, B));
    B = normalize(cross(T, N));

    float3 C1 = 0.0f;
    float3 C = 2.0f * map.Sample(state, uv * 2.5f).xyz - 1.0f;
    C = normalize(C);

   
    C1.x = B.x * C.x + T.x * C.y + N.x * C.z;
    C1.y = B.y * C.x + T.y * C.y + N.y * C.z;
    C1.z = B.z * C.x + T.z * C.y + N.z * C.z;
    C = normalize(C1);

    return C;
}

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = input.Normal;
    
    float3 incident = input.wPosition - ViewPosition;
    float3 reflection = 2.0f * dot(-incident, normal) * normal + incident;

    float4 color = CubeMap.Sample(Sampler, reflection); //  반사
    
    return float4(color.rgb, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////

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
}