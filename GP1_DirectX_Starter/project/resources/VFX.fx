float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
float4x4 gWorldMatrix : WorldMatrix;
float3 gCameraPosition : CameraPosition;
float PI = 3.14159;

RasterizerState gRasterizerState
{
    CullMode = none;
    FrontCounterClockwise = false;
};

SamplerState gSamplerState
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;

    // others are redundant because
    // StencilEnable is FALSE
    // (for demo purposes only)
    //StencilReadMask = 0x0F;
    //StencilWriteMask = 0x0F;
    //
    //FrontFaceStencilFunc = always;
    //BackFaceStencilFunc = always;
    //
    //FrontFaceStencilDepthFail = keep;
    //BackFaceStencilDepthFail = keep;
    //
    //FrontFaceStencilPass = keep;
    //BackFaceStencilPass = keep;
    //
    //FrontFaceStencilFail = keep;
    //BackFaceStencilFail = keep;
};

BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : UV;

};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;

};

// vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.position = mul(float4(input.position, 1.f), gWorldViewProj);
    output.uv = input.uv;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSamplerState, input.uv);
}

technique11 DefaultVFXTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}