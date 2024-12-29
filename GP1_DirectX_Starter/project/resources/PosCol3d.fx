
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
	float3 color : COLOR;
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
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(float4(input.position,1.f),gWorldViewProj);
	output.uv = input.uv;
	output.normal = input.normal;
	output.tangent = input.tangent;
	return output;
}

// pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float invViewDirection = normalize(gCameraPosition - input.worldPosition.xyz);
	

	float4x3 sampledTexture = SampleTextures(input);
	
	float3 sampledDiffuse = sampledTexture[0];
	float3 sampledSpecular = sampledTexture[1];
	float3 sampledGlossiness = sampledTexture[2];
	float3 sampledNormal = sampledTexture[3];

	float cosA = max(0.0f,dot(gLightDirection,sampledNormal));

	const float3 reflect = gLightDirection - 2* dot(sampledNormal,gLightDirection) * sampledNormal;
	const float phongCosA = max(0.0f,dot(invViewDirection, reflect) );
	const float3 PhongSpec = sampledSpecular *  pow(phongCosA,sampledGlossiness.r * Shininess);

	float3 lambertDiffuse = sampledDiffuse * 7.0f / PI;
 
	float3 finalColor = (lambertDiffuse + PhongSpec + float3(0.025f,0.025f,0.025f));
	finalColor *= cosA;
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