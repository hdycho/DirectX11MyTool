#include "000_EffectHeader.hlsl"

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

TextureCube CubeMap;
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.oPosition.xyz);

    //  방향과 픽셀의 충돌을 가지고 Sampling을 함
    return CubeMap.Sample(Sampler, normal);
}


DepthStencilState Depth
{
    //DepthEnable = false;
    DepthFunc = LESS_EQUAL;
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