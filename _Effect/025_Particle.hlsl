#include "000_EffectHeader.hlsl"


//============================================================
// States
//============================================================

struct ParticleDesc
{
    float3 Pos;
    float alphaValue;
    int IsActive;
    float Velocity;
    float2 Size;

    int animIdx;
    float3 padding;
};

ParticleDesc particles[1360];
float perFrame;

//============================================================
// VS
//============================================================
struct VertexOutput
{
    float4 Position : POSITION0;
    float2 Id : ID0;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;

    output.Position = input.Position;

    return output;
}

struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : Position1;
    float2 Uv : UV0;
    uint Id : ID0;
    float AlphaValue : ALPHA0;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS(point VertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, 
        inout TriangleStream<GeometryOutput> stream)
{
    //GS 단위에서 랜더자체를 안함
    if (!particles[PrimitiveID].IsActive)
        return;

    matrix trans ={ 1, 0, 0, 0, 0, 1, 0, 0,0, 0, 1, 0,
                    particles[PrimitiveID].Pos.x,particles[PrimitiveID].Pos.y,
                    particles[PrimitiveID].Pos.z,1};

    float4 transPos = mul(input[0].Position, trans);

    float3 up = float3(0, 1, 0);
    float3 look = ViewPosition - transPos.xyz;

    look = normalize(look);
    float3 right = cross(up, look);

    //너비,높이의 반지름
    float halfWidth = 0.5f * particles[PrimitiveID].Size.x;
    float halfHeight = 0.5f * particles[PrimitiveID].Size.y;
    
    // 정점하나로 빌보드 생성
    float4 v[4];
    v[0] = float4(transPos.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(transPos.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(transPos.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(transPos.xyz - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;

    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        output.wPosition = input[0].Position.xyz;

        output.Position = v[i];
        output.Position = mul(output.Position, View);
        output.Position = mul(output.Position, Projection);

        output.AlphaValue = particles[PrimitiveID].alphaValue;
        output.Uv = TexCoord[i];
        output.Id = PrimitiveID;
        stream.Append(output);
    }

}

Texture2DArray tex;
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
float4 PS(GeometryOutput input) : SV_TARGET
{
    //번호 0번 0번그림 = > TextureNumber
    //번호 1번 1번그림


    int num = particles[input.Id].animIdx;

    float3 uvw = float3(input.Uv, num);
    float4 particle = tex.Sample(Sampler, uvw);

    clip(particle.a - 0.5f);


    particle.a = input.AlphaValue;
    return particle;
}

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