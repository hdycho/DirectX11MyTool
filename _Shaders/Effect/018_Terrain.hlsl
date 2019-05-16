#include "000_EffectHeader.hlsl"

// --------------------------------------------------------------------- //
//  Global Variables
// --------------------------------------------------------------------- //
float Ratio = 20.0f;

// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //
struct VertexOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    return output;
}

// --------------------------------------------------------------------- //
//  Hull Shader
// --------------------------------------------------------------------- //
struct ConstantOutput
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsidetessFactor;
};

int ComputeAmount(float3 position)
{
    float dist = distance(position, ViewPosition);
    float s = saturate((dist - Ratio) / (100.0f - Ratio));

    return (int) lerp(6, 1, s);
}

ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchId : SV_PrimitiveId)
{
    ConstantOutput output;

    float3 center = 0.0f;
    center = (input[0].Position.xyz + input[1].Position.xyz) * 0.5f;
    output.Edges[0] = ComputeAmount(center);
    
    center = (input[1].Position.xyz + input[3].Position.xyz) * 0.5f;
    output.Edges[1] = ComputeAmount(center);

    center = (input[2].Position.xyz + input[3].Position.xyz) * 0.5f;
    output.Edges[2] = ComputeAmount(center);

    center = (input[0].Position.xyz + input[2].Position.xyz) * 0.5f;
    output.Edges[3] = ComputeAmount(center);
    
    center = (input[0].Position + input[1].Position + input[2].Position + input[3].Position) * 0.25f;
    output.Inside[0] = ComputeAmount(center);
    output.Inside[1] = output.Inside[0];

    return output;
}

struct HullOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

//  line
//  line_adj
//  tri
//  tri_adj
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
//[maxtessfactor]
HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveId)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;
    output.Normal = input[pointID].Normal;

    return output;
}

// --------------------------------------------------------------------- //
//  Domain Shader
// --------------------------------------------------------------------- //
struct DomainOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

[domain("quad")]
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;
    
    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y);
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y);
    float3 p = lerp(v1, v2, uv.x);
    
    output.Position = mul(float4(p, 1.0f), View);
    output.Position = mul(output.Position, Projection);
    
    float2 uv1 = lerp(patch[0].Uv.xy, patch[1].Uv.xy, 1 - uv.y);
    float2 uv2 = lerp(patch[2].Uv.xy, patch[3].Uv.xy, 1 - uv.y);
    output.Uv = lerp(uv1, uv2, uv.x);
    
    float3 n1 = lerp(patch[0].Normal, patch[1].Normal, 1 - uv.y);
    float3 n2 = lerp(patch[2].Normal, patch[3].Normal, 1 - uv.y);
    output.Normal = lerp(n1, n2, uv.x);

    return output;
}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(DomainOutput input) : SV_TARGET
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);
    diffuse *= dot(-LightDirection, normalize(input.Normal));

    return diffuse;
}

// --------------------------------------------------------------------- //
//  Technique
// --------------------------------------------------------------------- //
RasterizerState FillMode
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetRasterizerState(FillMode);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

}