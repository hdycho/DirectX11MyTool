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
    float2 Uv : UV0; //�ȼ��� ��ġ
};


Texture2D Map;
float alphaValue = 1;
int width;
int height;
int size;

VertexOutput VS(VertexTexture input)
{
    //W:������ǥ => NDC(ȭ��)������ ���� ����
    //SV_POSITION�� ��ȯ���� float4������
    //������ǥ�� �߰��Ǿ 
    //���ؽ����̴������� ������ 0�϶� ���� 1�϶� ��ġ
    //�ȼ����̴������� ������ ȭ����ǥ�� ���� ����
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    //NDC ���� - FVF_RHWXYZ
    //output.Position = input.Position;
    output.Uv = input.Uv;
    return output; // ��ȯ���� �ȼ��� ��ġ
}


//============================================================
// PS
//============================================================
float4 PS(VertexOutput input) : SV_TARGET
{
        //ȭ�� ũ�����
    float xRatio = 1.0f / (float) width;
    float yRatio = 1.0f / (float) height;

    //�ܺο��� ������ ����� �����γ���
    float2 mosaicRatio;
    mosaicRatio.x = size * xRatio;
    mosaicRatio.y = size * yRatio;

    //������ �߾���
    float2 mosaicCenter;
    mosaicCenter.x = mosaicRatio.x / 2.0f;
    mosaicCenter.y = mosaicRatio.y / 2.0f;

    //������ũuv��ǥ����
    //������ũ������ ���缭 ��������� ����
    float2 mosaicUvRatio;
    mosaicUvRatio.x = (int) (input.Uv.x / mosaicRatio.x);
    mosaicUvRatio.y = (int) (input.Uv.y / mosaicRatio.y);

    //���� uv��ǥ��ġ�� ���� ���������� �߾���ǥ
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