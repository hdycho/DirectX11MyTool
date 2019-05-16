#include "000_EffectHeader.hlsl"

float4 Center;
float4 Apex;
float Height;

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float4 oPosition : POSITION1;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

SamplerState Sampler : register(s0);

float4 PS(VertexOutput input) : SV_TARGET
{
    float y = saturate(input.oPosition.y);

    return lerp(Center, Apex, y * Height);
}

DepthStencilState Depth
{
    DepthEnable = false;
};

RasterizerState Cull
{
    FrontCounterClockwise = true;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
        SetRasterizerState(Cull);
    }
}