#include "000_EffectHeader.hlsl"


//=====================================================//
// VertexShader
//=====================================================//
struct VertexInput
{
    float4 Position : POSITION0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output; // 반환값이 픽셀의 위치
}


//=====================================================//
// PixelShader
//=====================================================//
float4 PS(VertexOutput input) : SV_TARGET
{
    return Color;
}


//=====================================================//
// technique
//=====================================================//
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}