
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
Texture2D gNormalMap : GlossinessMap;

float3 gLightDirection = { 0.577, -0.577, 0.577 };
float4x4 gWorldMatrix : WorldMatrix;
float3 gCameraPosition : CameraPosition;
float PI = 3.14159;
float LightIntesity = 7.0f;
float Shininess = 25.0f;

SamplerState gSamplerState
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState gDepthStencilState {}; // empty to reset depth stencil state after VFX

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLD;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

float4x3 SampleTextures(VS_OUTPUT input)
{
    const float4 SampledDiffuse = gDiffuseMap.Sample(gSamplerState, input.uv);
    const float4 SampledSpecular = gSpecularMap.Sample(gSamplerState, input.uv);
    const float4 SampledGlossiness = gGlossinessMap.Sample(gSamplerState, input.uv);
    const float4 SampledNormal = gNormalMap.Sample(gSamplerState, input.uv);

    float4x3 output = { SampledDiffuse.rgb, SampledSpecular.rgb, SampledGlossiness.rgb, SampledNormal.rgb };

    return output;
};

float3 CalculateLamberDiffuse(float3 lambertTexture)
{
    return lambertTexture * LightIntesity / PI;
}

float3 CalculatePhonSpec(float4x3 sampledTextures, float3 invViewDirection)
{
    const float3 ref = -normalize(reflect(gLightDirection, sampledTextures[3]));
    const float PhonCosA = max(0.f, dot(ref, invViewDirection));
		
    return sampledTextures[1] * pow(PhonCosA, sampledTextures[2].r * Shininess);

}
// vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.position = mul(float4(input.position, 1.f), gWorldViewProj);
    output.worldPosition = mul(float4(input.position, 1.f), gWorldMatrix);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), gWorldMatrix).xyz;
    output.tangent = mul(float4(input.tangent, 0.f), gWorldMatrix).xyz;
    return output;
}

// pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	
    const float3 invViewDirection = normalize(gCameraPosition - input.worldPosition.xyz);
    float3 finalColor;
    const float4x3 SampledTextures = SampleTextures(input);
	
    const float3 LambertDiffuse = CalculateLamberDiffuse(SampledTextures[0]);
    const float3 PhongSpec = CalculatePhonSpec(SampledTextures, invViewDirection);
	
    const float3 BiNormal = cross(input.normal, input.tangent);
    float4x3 tangentSpaceAxis = { input.tangent, BiNormal, input.normal, (float3)0 };
    float3 SampledNormal = 2 * SampledTextures[3] - float3(1, 1, 1);
    
    SampledNormal = mul(SampledNormal, tangentSpaceAxis);
	
    const float cosA = max(0.f, dot(-gLightDirection, SampledNormal));
    finalColor = LambertDiffuse + PhongSpec + float3(0.025f, 0.025f, 0.025f);
    finalColor *= cosA;
    return float4(finalColor, 1.f);
}

technique11 DefaultTechnique
{
    pass P0
    {
        SetDepthStencilState(gDepthStencilState, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}