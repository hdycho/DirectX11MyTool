#include "000_EffectHeader.hlsl"


//============================================================
// States
//============================================================


float frameTime;
float3 scrollSpeeds;
float3 scales;
float padding;


DepthStencilState Depth
{
    DepthEnable = false;
};

BlendState Blend
{
    AlphaToCoverageEnable = true;

    BlendEnable[0] = true;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A

    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = Zero;
    BlendOpAlpha[0] = Add;
};


//============================================================
// VS
//============================================================
struct VertexOutput
{
    float4 Position : POSITION0;
    float2 Tex : TEXTURE0;
    float2 TexCoords1 : TEXTURE1;
    float2 TexCoords2 : TEXTURE2;
    float2 TexCoords3 : TEXTURE3;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;

    input.Position.w = 1.0f;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Tex = input.Uv;

    output.TexCoords1 = (input.Uv * scales.x);
    output.TexCoords1.y = output.TexCoords1.y + (frameTime * scrollSpeeds.x);

    output.TexCoords2 = (input.Uv * scales.x);
    output.TexCoords2.y = output.TexCoords2.y + (frameTime * scrollSpeeds.y);

    output.TexCoords3 = (input.Uv * scales.z);
    output.TexCoords3.y = output.TexCoords3.y + (frameTime * scrollSpeeds.z);

    return output;
}

Texture2D fireTex;
Texture2D noiseTex;
Texture2D alphaTex;

float2 distortion1;
float2 distortion2;
float2 distortion3;
float distortionScale;
float distortionBias;

sampler Sampler
{
    AddressU = WRAP;
    AddressV = WRAP;
    Filter = COMPARISON_ANISOTROPIC;
    MaxAnisotropy = 16;
    MaxLOD = 2.0f;
};

//============================================================
// PS
//============================================================
float4 PS(VertexOutput input) : SV_TARGET
{
    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;
    
}




//============================================================
// technique11
//============================================================
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        //SetDepthStencilState(Depth, 0);
        SetBlendState(Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}