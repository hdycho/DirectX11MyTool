#include "001_header2.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    float3 Instance : INSTANCE0;
    uint InstanceId : SV_INSTANCEID;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    uint InstanceId : INSTANCEID0;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position.x = input.Position.x + input.Instance.x;
    output.Position.y = input.Position.y + input.Instance.y;
    output.Position.z = input.Position.z + input.Instance.z;
    output.Position.w = 1.0f;

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    output.InstanceId = input.InstanceId;

    return output;

}

Texture2D Map[6] : register(t10);
SamplerState Sampler : register(s10);

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map[0].Sample(Sampler, input.Uv);

    return color;
}