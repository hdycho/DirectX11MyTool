#include "000_EffectHeader.hlsl"


//============================================================
// States
//============================================================
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};
DepthStencilState Depth
{
    DepthEnable = false;
};

BlendState Blend
{
    AlphaToCoverageEnable = false;

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
    float4 Position : SV_POSITION; 
    float2 Uv : UV0; //픽셀의 위치
};


Texture2D Map;
float alphaValue = 1;

VertexOutput VS(VertexTexture input)
{
    //W:동차좌표 => NDC(화면)공간에 대한 비율
    //SV_POSITION의 반환값이 float4인이유
    //동차좌표가 추가되어서 
    //버텍스쉐이더에서의 동차는 0일때 방향 1일때 위치
    //픽셀쉐이더에서의 동차는 화면좌표에 대한 비율
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    //NDC 공간 - FVF_RHWXYZ
    //output.Position = input.Position;
    output.Uv = input.Uv;
    return output; // 반환값이 픽셀의 위치
}


//============================================================
// PS
//============================================================
float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);

    clip(color.a - 0.1f);

    return color;
}




//============================================================
// technique11
//============================================================
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
        SetBlendState(Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}