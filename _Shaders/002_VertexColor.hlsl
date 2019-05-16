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

struct VertexInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
	//�ȼ��� ��ġ
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.color = input.color;

    return output; // ��ȯ���� �ȼ��� ��ġ
}

float4 PS(PixelInput input) : SV_TARGET
{
	//���� float4 = > r,g,b,a
    return input.color;
}