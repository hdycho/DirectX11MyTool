#include "000_EffectHeader.hlsl"

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : TEX0;
};


VertexOutput VS(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}

float4 PS(VertexOutput input) : SV_TARGET
{
	//색은 float4 = > r,g,b,a
    return float4(1, 0, 0, 1);
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