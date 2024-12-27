#pragma once
#include "pch.h"
#include "Matrix.h"
#include "Texture.h"


class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& effectAsset);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;


	//static ID3DX11Effect* GetCurrentEffect() const { return m_pCurrentEffect; }

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	
	void SetDiffuseMap(Texture* pDiffuseTexture);

	ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }

	void SetMatrix(const float* m);

	void ChangeSampler(ID3D11Device* m_pDevice, D3D11_FILTER filter);
private:
	

	ID3DX11Effect* m_pCurrentEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	ID3DX11EffectSamplerVariable* m_pSamplerVariable{};
	ID3D11SamplerState* m_pSamplerState{};
};



