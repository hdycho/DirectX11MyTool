#include "001_header2.hlsl"

struct VertexOutput
{
    float3 Center : POSITION;
    float2 Size : Size;
};

VertexOutput VS(VertexSize input)
{
    VertexOutput output;

    output.Center = input.Position.xyz;
    output.Size = input.Size;
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float3 cPosition : POSITION1;
    uint Id : SV_PRIMITIVEID;
};

static const float2 TexCoord[4] =
{
    float2(0, 1),
    float2(0, 0),
    float2(1, 1),
    float2(1, 0)
};

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS(point VertexOutput input[1], uint Id : SV_PRIMITIVEID, inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 look = CameraPosition() - input[0].Center;

    //look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look);

    //너비,높이의 반지름
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
    float4 v[4];
    v[0] = float4(input[0].Center + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(v[i], World);
        output.wPosition = output.Position;

        output.Position = mul(output.Position, View);
        output.Position = mul(output.Position, Projection);

        output.Normal = look;
        output.Uv = TexCoord[i];
        output.cPosition = CameraPosition();

        output.Id = Id;

        stream.Append(output);
    }
}
///////////////////////////////////////////////////////////////////////////////

Texture2DArray Map : register(t10);
SamplerState Sampler : register(s10);

float4 PS(GeometryOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.Id % 6);

    float4 color = Map.Sample(Sampler, uvw);

    clip(color.a - 0.5); // 클리핑

    return color;
}