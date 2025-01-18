#include "BaseEffectClass.h"



BaseEffectClass::BaseEffectClass()
{

}

BaseEffectClass::~BaseEffectClass()
{


	if (m_pRasterizerState)
	{
		m_pRasterizerState->Release();
	}
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
	}
	if (m_pRasterizerVariable)
	{
		m_pRasterizerVariable->Release();
	}
	if (m_pSamplerVariable)
	{
		m_pSamplerVariable->Release();
	}
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->Release();
	}
	if (m_pWorldMatrix)
	{
		m_pWorldMatrix->Release();
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


ID3DX11Effect* BaseEffectClass::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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


void BaseEffectClass::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	}
}

void BaseEffectClass::SetCameraPosition(const float* pData)
{
	if (m_pCameraPosition)
	{
		m_pCameraPosition->SetRawValue(pData, 0, 12);
	}
}

void BaseEffectClass::SetWorldPosition(const float* pData)
{
	if (m_pWorldMatrix)
	{
		m_pWorldMatrix->SetMatrix(pData);
	}
}

void BaseEffectClass::SetWorldViewMatrix(const float* m)
{
	m_pMatWorldViewProjVariable->SetMatrix(m);

}

void BaseEffectClass::ChangeSampler(ID3D11Device* m_pDevice, D3D11_FILTER filter)
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

void BaseEffectClass::ChangeDirectXCullingMode(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode)
{
	if (m_pRasterizerState)
	{
		m_pRasterizerState->Release();
	}

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	HRESULT result = m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
	if (FAILED(result)) return;

	m_pRasterizerVariable = m_pCurrentEffect->GetVariableByName("gCullingMode")->AsRasterizer();
	m_pRasterizerVariable->SetRasterizerState(0, m_pRasterizerState);

}
