#include "000_EffectHeader.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    matrix World : INSTANCE0;
    uint Texture : INSTANCE4;

    uint InstanceId : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    uint Texture : TEXTURE0;

    uint InstanceId : INSTANCEID0;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = mul(input.Position, input.World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    output.Texture = input.Texture;
    output.InstanceId = input.InstanceId;

    return output;
}

Texture2DArray Map;
SamplerState Sampler;

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.Texture);
    float4 color = Map.Sample(Sampler, uvw);
    
    return color;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}