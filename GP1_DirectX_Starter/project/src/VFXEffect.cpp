#include "VFXEffect.h"



VFXEffect::VFXEffect(ID3D11Device* pDevice, const std::wstring& effectAsset)
{

	m_pCurrentEffect = BaseEffectClass::LoadEffect(pDevice, effectAsset);
	m_pTechnique = m_pCurrentEffect->GetTechniqueByName("DefaultVFXTechnique");
	if (!GetTechnique()->IsValid())
	{
		std::wcout << L"Invalid technique\n";
	}

	// vertex layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "UV";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 24;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


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
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
	}


	m_pDiffuseMapVariable = m_pCurrentEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid\n";

	}

	m_pSamplerVariable = m_pCurrentEffect->GetVariableByName("gSamplerState")->AsSampler();
	if (!m_pSamplerVariable->IsValid())
	{
		std::wcout << L"m_pSamplerState not valid\n";

	}
	m_pWorldMatrix = m_pCurrentEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix)
	{
		std::wcout << L"m_pWorldMatrix not valid\n";

	}
	m_pCameraPosition = m_pCurrentEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPosition)
	{
		std::wcout << L"m_pCameraPosition not valid\n";
	}

	
	
}
