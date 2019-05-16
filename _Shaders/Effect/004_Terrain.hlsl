#include "000_EffectHeader.hlsl"

//=====================================================//
// VertexShader
//=====================================================//

struct VertexOutput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = input.Normal;
    output.Uv = input.Uv;

    return output; // 반환값이 픽셀의 위치
}


//=====================================================//
// PixelShader
//=====================================================//
SamplerState Sampler;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);

    float NdotL = dot(-LightDirection, normalize(input.Normal));

    return diffuse * NdotL;
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