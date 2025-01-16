#pragma once
#include "BaseEffectClass.h"



class Effect : public BaseEffectClass
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& effectAsset);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	
	void SetGlossMap(Texture* pDiffuseTexture) override;
	void SetSpecularMap(Texture* pSpecularTexture) override;
	void SetNormalMap(Texture* pNormalTexture) override;


private:

	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};

};



