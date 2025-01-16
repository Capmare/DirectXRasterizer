#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& effectAsset)
{
	// load effect and get technique for rendering
	m_pCurrentEffect = Effect::LoadEffect(pDevice, effectAsset);
	m_pTechnique = m_pCurrentEffect->GetTechniqueByName("DefaultTechnique");
	if (!GetTechnique()->IsValid())
	{
		std::wcout << L"Invalid technique\n";
	}

	// vertex layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "UV";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 24;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 32;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 44;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// create input layout
	D3DX11_PASS_DESC pasDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&pasDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		pasDesc.pIAInputSignature,
		pasDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result)) return;

	m_pMatWorldViewProjVariable = m_pCurrentEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid";
	}


	m_pDiffuseMapVariable = m_pCurrentEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid";

	}

	m_pSpecularMapVariable = m_pCurrentEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable not valid";

	}
	
	m_pGlossinessMapVariable = m_pCurrentEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable not valid";

	}

	m_pNormalMapVariable = m_pCurrentEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable not valid";

	}

	m_pSamplerVariable = m_pCurrentEffect->GetVariableByName("gSamplerState")->AsSampler();
	if (!m_pSamplerVariable->IsValid())
	{
		std::wcout << L"m_pSamplerState not valid";

	}
	m_pWorldMatrix = m_pCurrentEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix)
	{
		std::wcout << L"m_pWorldMatrix not valid";

	}
	m_pCameraPosition = m_pCurrentEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPosition)
	{
		std::wcout << L"m_pCameraPosition not valid";
	}

	


}

Effect::~Effect()
{


	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->Release();
	}
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->Release();
	}
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->Release();
	}
	if (m_pMatWorldViewProjVariable)
	{
		m_pMatWorldViewProjVariable->Release();
	}

}



void Effect::SetGlossMap(Texture* pGlossTexture)
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossTexture->GetSRV());
	}
}

void Effect::SetSpecularMap(Texture* pSpecularTexture)
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularTexture->GetSRV());
	}
}

void Effect::SetNormalMap(Texture* pNormalTexture)
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalTexture->GetSRV());
	}
}




