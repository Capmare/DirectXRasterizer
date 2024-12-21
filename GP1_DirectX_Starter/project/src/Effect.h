#pragma once
#include "pch.h"


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

	ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }
private:
	
	ID3D11Device* m_pDevice;
	ID3DX11Effect* m_pCurrentEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

};



