
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
Texture2D gNormalMap : GlossinessMap;

float3 gLightDirection = {0.577,-0.577,0.577};
float4x4 gWorldMatrix : WorldMatrix;
float3 gCameraPosition : CameraPosition;
float PI = 3.1415;
float LightIntesity = 7.0f;
float Shininess = 25.0f;

SamplerState gSamplerState
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

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

float4x3 SampleTextures(VS_OUTPUT input){

	float4 SampledDiffuse = gDiffuseMap.Sample(gSamplerState, input.uv);
	float4 SampledSpecular = gSpecularMap.Sample(gSamplerState, input.uv);
	float4 SampledGlossiness = gGlossinessMap.Sample(gSamplerState, input.uv);
	float4 SampledNormal = gNormalMap.Sample(gSamplerState, input.uv);

	float4x3 output = {SampledDiffuse.rgb,SampledSpecular.rgb,SampledGlossiness.rgb,SampledNormal.rgb};

	return output;
};

float3 CalculateSpecular(VS_INPUT input)
{

	return float3(0,0,0);
};

// vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
    output.position	= mul(float4(input.position, 1.f), gWorldViewProj);
    output.worldPosition = mul(float4(input.position, 1.f), gWorldMatrix);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal , 0.f), gWorldMatrix).xyz;
    output.tangent = mul(float4(input.tangent, 0.f), gWorldMatrix).xyz;
    
    return output;
}

float3 CalculateLambertDiffuse(VS_OUTPUT input)
{
    float3 colorDiffuse = gDiffuseMap.Sample(gSamplerState, input.uv).xyz;
    return (LightIntesity * colorDiffuse / PI);
}

float3 CalculateSampledNormal(VS_OUTPUT input)
{
    const float3 sampledNormal = 2 * gNormalMap.Sample(gSamplerState, input.uv).xyz - float3(1, 1, 1);
    const float3 binormal = cross(input.normal, input.tangent);
    
    float4x4 tangentSpaceAxis = (float4x4) 0;
    tangentSpaceAxis[0] = float4(input.tangent, 0.f);
    tangentSpaceAxis[1] = float4(binormal, 0.f);
    tangentSpaceAxis[2] = float4(input.normal, 0.f);
    tangentSpaceAxis[3] = float4((float3) 0, 1.f);

    return mul(float4(sampledNormal, 0.f), tangentSpaceAxis).xyz;
}

float3 CalculatePhong(VS_OUTPUT input, float3 viewDirection, float3 normal)
{
    const float3 ref        = -normalize(reflect(gLightDirection, normal));
    const float  cos      = max(dot(ref, viewDirection), 0.f);
    const float3 specular = gSpecularMap.Sample(gSamplerState, input.uv).xyz;
    const float  gloss    = gGlossinessMap.Sample(gSamplerState, input.uv).r;
    
    return specular * pow(cos, gloss * Shininess);
}



// pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float3 invViewDirection = normalize(input.worldPosition.xyz - gCameraPosition);

	const float3 lambertDiffuse = CalculateLambertDiffuse(input);
	const float3 normal = CalculateSampledNormal(input);
	const float3 phong = CalculatePhong(input,invViewDirection,normal);
	const float cosA = max(dot(-gLightDirection,normal),0.f);

	const float3 finalColor = (lambertDiffuse + phong + float3(0.025f,0.025f,0.025f) * cosA);
	return float4(finalColor,1.f);
}

technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
	}
}