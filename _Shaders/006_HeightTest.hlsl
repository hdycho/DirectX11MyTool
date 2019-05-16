cbuffer VS_ViewProjection : register(b0)
{
	//임시 고속저장장치 : 레지스터 (용량이작다)
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
                      //SV만 정해진 예약어임-> 시멘틱
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
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
	//색은 float4 = > r,g,b,a
    //return Color;
    return input.color;
}