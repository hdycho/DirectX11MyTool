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
int width;
int height;
int size;

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
        //화면 크기비율
    float xRatio = 1.0f / (float) width;
    float yRatio = 1.0f / (float) height;

    //외부에서 설정한 사이즈를 비율로나눔
    float2 mosaicRatio;
    mosaicRatio.x = size * xRatio;
    mosaicRatio.y = size * yRatio;

    //비율의 중앙점
    float2 mosaicCenter;
    mosaicCenter.x = mosaicRatio.x / 2.0f;
    mosaicCenter.y = mosaicRatio.y / 2.0f;

    //모자이크uv좌표비율
    //모자이크비율에 맞춰서 실제사이즈값 나옴
    float2 mosaicUvRatio;
    mosaicUvRatio.x = (int) (input.Uv.x / mosaicRatio.x);
    mosaicUvRatio.y = (int) (input.Uv.y / mosaicRatio.y);

    //실제 uv좌표위치가 속한 비율영역의 중앙좌표
    float2 uvCenter = mosaicUvRatio * mosaicRatio + mosaicCenter;
     

    float4 color = Map.Sample(Sampler, uvCenter);

    color.a = alphaValue;
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