struct VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};


// vertex shader

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = float4(input.position,1.f);
	output.color = input.color;
	return output;
}

// pixel shader

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.color,1.f);
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