struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct VS_OUTPUT
{
	float3 Position : SV_POSITION;
	float3 Color : COLOR;
};


// vertex shader

VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = float4(input.Position,1.f);
	output.Color = input.Color;
	return output;
}

// pixel shader

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color,1.f);
}

technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader(CompilerShader(vs_5_0,VS()));
		SetGeomtryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0,PS()));
	}
}