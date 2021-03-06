#include "000_EffectHeader.hlsl"

// Our constants
static const float kHalfPi = 1.5707;
static const float kQuarterPi = 0.7853;
static const float kOscillateDelta = 0.5;
static const float kWindCoeff = 50.0f;

float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}

float3 HSVtoRGB(in float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1) * HSV.y + 1) * HSV.z;
}


float4 WorldFrustumPlanes[6];
float FogStart;
float FogRange;

// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //
struct VertexInput
{
    float4 Position : POSITION0;
    
    float3 TransPosition : INSTANCE0;
    matrix DeTransWorld : INSTANCE1;
    matrix RotateWorld : INSTANCE5;
    float2 Size : INSTANCE9;
    uint TextureNumber : INSTANCE10;

    uint InstanceId : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Center : POSITION0;
    float2 Size : SIZE0;
    uint TextureNumber : GRASS0;
    matrix RotateWorld : MATRIX0;
    matrix DeTransWorld : MATRIX4;
    uint InstanceId : INSTANCEID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Center = float4(input.TransPosition, 1);

    output.Size = input.Size;
    output.TextureNumber = input.TextureNumber;
    output.RotateWorld = input.RotateWorld;
    output.DeTransWorld = input.DeTransWorld;
    output.InstanceId = input.InstanceId;
    
    return output;
}

// --------------------------------------------------------------------- //
//  Geometry Shader
// --------------------------------------------------------------------- //
struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : Position1;
    float2 Uv : UV0;
    uint TextureNumber : GRASS0;
    float4 ShadowTransform : UV1;
    uint InstanceId : INSTANCEID0;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};

bool AabbOutsideFrustumTest(float3 center, float3 extents)
{
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
      // If the box is completely behind any of the frustum planes
      // then it is outside the frustum.
        [flatten]
        if (AabbBehindPlaneTest(center, extents, WorldFrustumPlanes[i]))
        {
            return true;
        }
    }
    return false;
}

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS(point VertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float4 root = input[0].Center;

    // Generate a random number between 0.0 to 1.0 by using the root position (which is randomized by the CPU)
    float random = sin(kHalfPi * frac(root.x) + kHalfPi * frac(root.z));
    float randomRotation = random;
    float cameraDistance = length(ViewPosition.xz - root.xz);

    // Properties of the grass blade
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
	// Animation
    float toTheLeft = sin(Time);

    // Rotate in Z-axis
    float3x3 rotationMatrix =
    {
        cos(randomRotation), 0, sin(randomRotation),
		0, 1, 0,
		-sin(randomRotation), 0, cos(randomRotation)
    };


    // Generating vertices
    float3 up = float3(0, 1, 0);
    float3 look = ViewPosition - input[0].Center.xyz;
    look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look); 

    
    float3 boxExtents = (halfWidth + halfHeight) * 0.5f;
    if (AabbOutsideFrustumTest(input[0].Center.xyz, boxExtents))
    {
        return;
    }
    //  4 정점 계산
    float4 v[4];

    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);

    // Wind
    float windCoEff = 0;
    float steepnessFactor = 1.75;

    GeometryOutput output;

    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        if (input[0].TextureNumber != 2)
        {
            output.wPosition = input[0].Center.xyz;

            output.Position = mul(v[i], input[0].RotateWorld);
            output.Position = mul(output.Position, input[0].DeTransWorld);
            output.Position = mul(output.Position, View);
            output.Position = mul(output.Position, Projection);

            output.ShadowTransform = mul(float4(output.wPosition, 1), ShadowTransform);
  
            output.Uv = TexCoord[i];
            output.TextureNumber = input[0].TextureNumber;
            output.InstanceId = input[0].InstanceId;

            stream.Append(output);
        }
        else
        {
             //Wind 
            float2 windVec = float2(0.3f, 0);
            windVec.x += (sin(Time + root.x / 25) + sin((Time + root.x / 15) + 50)) * 0.5;
            windVec.y += cos(Time + root.z / 80);
            windVec *= lerp(0.7, 1.0, 1.0 - random);

        // Oscillate wind
            float sinSkewCoeff = random;
            float oscillationStrength = 2.5f;
            float lerpCoeff = (sin(oscillationStrength * Time + sinSkewCoeff) + 1.0) / 2;
            float2 leftWindBound = windVec * (1.0 - kOscillateDelta);
            float2 rightWindBound = windVec * (1.0 + kOscillateDelta);
            windVec = lerp(leftWindBound, rightWindBound, lerpCoeff);

        // Randomize wind by adding a random wind vector
            float randAngle = lerp(-3.14, 3.14, random);
            float randMagnitude = lerp(0, 1.0, random);
            float2 randWindDir = float2(sin(randAngle), cos(randAngle));
            windVec += randWindDir * randMagnitude;

            float windForce = length(windVec);

            float currentV = 1;
            float VOffset = 0.5f;
            float currentNormalY = 0;
            float currentHeightOffset = sqrt(input[0].Size.y);
            float currentVertexHeight = 0;

            if (i % 2 != 0)
            {
			// General
                currentV -= VOffset;
                currentNormalY += VOffset * 2;

			// Height
                currentHeightOffset -= VOffset;
                float currentHeight = input[0].Size.y - (currentHeightOffset * currentHeightOffset);
                currentVertexHeight = currentHeight;

			// Wind
                windCoEff += VOffset; // TODO: Check these values

                //saturate(windCoEff);
            }

        //Calculate final vertex position based on wind
            v[i].xz += windVec * windCoEff;
        //v[i].y -= windForce * windCoEff*0.8f;
            //v[i].y += halfHeight * 1.3f;

            output.wPosition = input[0].Center.xyz;

            output.Position = mul(v[i], input[0].RotateWorld);
            output.Position = mul(output.Position, input[0].DeTransWorld);
            output.Position = mul(output.Position, View);
            output.Position = mul(output.Position, Projection);

         
            output.ShadowTransform = mul(float4(output.wPosition, 1), ShadowTransform);
  
            output.Uv = TexCoord[i];
            output.TextureNumber = input[0].TextureNumber;
            output.InstanceId = input[0].InstanceId;

            stream.Append(output);
        }
    }
}

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS_Depth(point VertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float4 root = input[0].Center;

    float cameraDistance = length(ViewPosition.xz - root.xz);

    // Properties of the grass blade
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
	// Animation
    float toTheLeft = sin(Time);


    // Generating vertices
    float3 up = float3(0, 1, 0);
    float3 lightPos = -LightDirection * 500.0f;
    float3 look = lightPos - input[0].Center.xyz;
    look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look);

    //  4 정점 계산
    float4 v[4];

    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);


    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        //output.Position = mul(v[i], LightView);
        //output.Position = mul(output.Position, LightProjection);
     
        //output.Uv = TexCoord[i];
        //output.BillboardNum = input[0].BillboardNum;

        //stream.Append(output);

        output.wPosition = v[i].xyz;
        output.Position = mul(v[i], LightView);
        output.Position = mul(output.Position, LightProjection);

        output.ShadowTransform = mul(v[i], ShadowTransform);
        
        output.Uv = TexCoord[i];
        output.TextureNumber = input[0].TextureNumber;
        output.InstanceId = input[0].InstanceId;

        stream.Append(output);
    }

}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
Texture2DArray Map;
sampler Sampler
{
    AddressU = WRAP;
    AddressV = WRAP;
    Filter = COMPARISON_ANISOTROPIC;
    MaxAnisotropy = 16;
    MaxLOD = 2.0f;
};

float4 PS(GeometryOutput input) : SV_TARGET
{
    //번호 0번 0번그림 = > TextureNumber
    //번호 1번 1번그림

    float3 uvw;
    if(input.TextureNumber==0)
        uvw = float3(input.Uv, 0);
    else if (input.TextureNumber == 1)
        uvw = float3(input.Uv, 1);
    else
        uvw = float3(input.Uv, 2);

    float4 flower = Map.Sample(Sampler, uvw);

    float3 toEye = ViewPosition - input.wPosition;
    float distanceToEye = length(toEye);
    
    toEye = normalize(toEye);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    float shadow = float3(1.0f, 1.0f, 1.0f);
    shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowTransform,input.wPosition);

    shadow = min(0.5f, shadow);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };


    float4 A, D, S;
    float3 normal = float3(0, 1, 0);
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += D * shadow;
    specular += S *shadow;
    
    float4 color = float4(ambient + diffuse + specular, 1.0f);
    color *= flower;

    clip(color.a - 0.5f);
    float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

    color = lerp(color, SunColor, fogFactor);

    return color;
}

void PS_Depth(GeometryOutput input)
{
    float3 uvw;
    if (input.TextureNumber == 0)
        uvw = float3(input.Uv, 0);
    else if (input.TextureNumber == 1)
        uvw = float3(input.Uv, 1);
    else
        uvw = float3(input.Uv, 2);

    float4 color = Map.Sample(Sampler, uvw);

    // Phong
    //float3 r = normalize(reflect(input.VertexToLight, input.Normal));
    //float shininess = 100;

    //float ambientLight = 0.1;
    //float diffuseLight = saturate(dot(input.VertexToLight, input.Normal));
    //float specularLight = saturate(dot(-input.VertexToCamera, r));
    //specularLight = saturate(pow(specularLight, shininess));
	
    //float light = ambientLight + (diffuseLight * 1.55)+(specularLight * 0.5);
	
    //float3 grassColorRGB = HSVtoRGB(grassColorHSV);

    clip(color.a - 0.5f);
}

// --------------------------------------------------------------------- //
//  Techniques
// --------------------------------------------------------------------- //
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Depth()));
        //SetPixelShader(CompileShader(ps_5_0, PS_Depth_Alpha()));
        SetPixelShader(NULL);

        SetRasterizerState(ShadowDepth);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Depth()));
        SetPixelShader(CompileShader(ps_5_0, PS_Depth()));
    }
}