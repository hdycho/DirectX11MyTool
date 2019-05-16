cbuffer VS_ViewProjection : register(b0)
{
	//�ӽ� ���������ġ : �������� (�뷮���۴�)
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Color : register(b0)
{
    float4 Color;
}

cbuffer PS_UVSET : register(b1)
{
    float uv;
    float idx;
}


struct VertexInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PixelInput
{
                      //SV�� ������ �������-> �ø�ƽ
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);


PixelInput VS(VertexInput input)
{
    PixelInput output;


    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.color = float4(1, 1, 1, 1);
    if (input.color.r > 0 && input.color.r <= 1)
        output.color = input.color;
    if (input.color.g > 0 && input.color.g <= 1)
        output.color = input.color;
    if (input.color.b > 0 && input.color.b <= 1)
        output.color = input.color;
    return output; // ��ȯ���� �ȼ��� ��ġ
}

float4 PS(PixelInput input) : SV_TARGET
{
	//���� float4 = > r,g,b,a
    //return Color;
    return input.color;
}