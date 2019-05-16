#include "000_EffectHeader.hlsl"


//=====================================================//
// VertexShader
//=====================================================//
struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : Position1;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS_Bone(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.Uv = input.Uv;
    

    return output; // 반환값이 픽셀의 위치
}

//=====================================================//
// PixelShader
//=====================================================//
//SamplerState Sampler; // 기본값사용시

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normalMap = NormalMap.Sample(Sampler, input.Uv);
    
    float3 normal = 0;

    float temp = saturate(dot(normalMap, 1));

    [flatten]
    if (temp > 0.0f)
        normal = NormalSampleToWorldSpace(normalMap, input.Normal, input.Tangent);
    else
        normal = input.Normal;
    
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    //float shadow = float3(1.0f, 1.0f, 1.0f);
    //shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowPos);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { { LightAmbient.r + 0.3, LightAmbient.g + 0.3, LightAmbient.b + 0.3, LightAmbient.a }, LightDiffuse, LightSpecular, LightDirection };

    float4 specularMap = SpecularMap.Sample(Sampler, input.Uv);

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += /*shadow * */D;
    specular += /*shadow **/S;
    

    temp = saturate(dot(specularMap, 1));
    if (temp > 0.0f)
        specular *= specularMap;
       
    [unroll]
    for (int i = 0; i < PointLightCount; i++)
    {
        ComputePointLight(m, PointLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    [unroll]
    for (i = 0; i < SpotLightCount; i++)
    {
        ComputeSpotLight(m, SpotLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    float4 color = float4(ambient + diffuse + specular, 1);


    float4 diffuseMap = DiffuseMap.Sample(Sampler, input.Uv);

    temp = saturate(dot(diffuseMap, 1));

    if (temp > 0.0f)
        color *= diffuseMap;

    color.a = Diffuse.a;

    return color;
}

BlendState BlendOn
{
    BlendEnable[0] = true;
    BlendOp[0] = Add;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = Inv_Src_Alpha;
    DestBlendAlpha[0] = Zero;
    SrcBlend[0] = Src_Alpha;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A
};

//=====================================================//
// technique
//=====================================================//
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Bone()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetBlendState(BlendOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}