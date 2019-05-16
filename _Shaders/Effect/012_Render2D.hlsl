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


//============================================================
// VS
//============================================================
struct VertexOutput
{
    float4 Position : SV_POSITION; 
    float2 Uv : UV0; //�ȼ��� ��ġ
};


Texture2D Map;

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
    float4 color = Map.Sample(Sampler, input.Uv);

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
    }
}