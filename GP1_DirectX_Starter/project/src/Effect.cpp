#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& effectAsset)
{
	// load effect and get technique for rendering
	m_pCurrentEffect = LoadEffect(pDevice, effectAsset);
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
	if (m_pSamplerVariable)
	{
		m_pSamplerVariable->Release();
	}
	if (m_pWorldMatrix)
	{
		m_pWorldMatrix->Release();
	}
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
	}
	if (m_pSamplerVariable)
	{
		m_pSamplerVariable->Release();
	}
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
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->Release();
	}
	if (m_pMatWorldViewProjVariable)
	{
		m_pMatWorldViewProjVariable->Release();
	}
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
	}
	if (m_pTechnique)
	{
		m_pTechnique->Release();
	}
	if (m_pCurrentEffect)
	{
		m_pCurrentEffect->Release();
	}

	
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile
	(
		assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob
	);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{}; i < pErrorBlob->GetBufferSize(); ++i)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Filed to createEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}

	}
	return pEffect;
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
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

void Effect::SetCameraPosition(const float* pData)
{
	if (m_pCameraPosition)
	{
		m_pCameraPosition->SetRawValue(pData,0,12);
	}
}

void Effect::SetWorldPosition(const float* pData)
{
	if (m_pWorldMatrix)
	{
		m_pWorldMatrix->SetMatrix(pData);
	}
}

void Effect::SetMatrix(const float* m)
{
	
	m_pMatWorldViewProjVariable->SetMatrix(m);
}

void Effect::ChangeSampler(ID3D11Device* m_pDevice, D3D11_FILTER filter)
{
	// Change sampler state
	if (m_pSamplerState)
	{
		m_pSamplerState->Release(); // release previous memory
	}
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;                        // using for anisotropy only
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(result)) return;

	m_pSamplerVariable->SetSampler(0, m_pSamplerState);
}
