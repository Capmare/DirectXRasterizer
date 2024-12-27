
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
SamplerState gSampler : register(s0);

//SamplerState MeshTextureSampler
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = Wrap;
//    AddressV = Wrap;
//};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
	float2 uv : UV;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 color : COLOR0;
	float2 uv : TEXCOORD0;

};


// vertex shader

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(float4(input.position,1.f),gWorldViewProj);
	output.color = input.color;
	output.uv = input.uv;
	return output;
}

// pixel shader
//  

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(gSampler, input.uv);
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