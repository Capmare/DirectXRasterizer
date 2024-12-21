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
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
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

}

Effect::~Effect()
{
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