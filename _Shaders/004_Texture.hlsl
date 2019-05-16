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

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
//픽셀의 위치
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
	//색은 float4 = > r,g,b,a
    //return Color;
    //이미지를 화면에 출력할때 조정하는 단계 Sample
    float4 color = Map.Sample(Sampler, input.uv);

    return color;
}