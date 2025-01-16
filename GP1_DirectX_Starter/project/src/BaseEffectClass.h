#pragma once
#include "pch.h"
#include "Matrix.h"
#include "Texture.h"

class BaseEffectClass
{
public:
	BaseEffectClass();
	~BaseEffectClass();
	
	BaseEffectClass(const BaseEffectClass&) = delete;
	BaseEffectClass(BaseEffectClass&&) noexcept = delete;
	BaseEffectClass& operator=(const BaseEffectClass&) = delete;
	BaseEffectClass& operator=(BaseEffectClass&&) noexcept = delete;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);


	virtual void SetDiffuseMap(Texture* pDiffuseTexture);
	virtual void SetGlossMap(Texture* pDiffuseTexture) {};
	virtual void SetSpecularMap(Texture* pSpecularTexture) {};
	virtual void SetNormalMap(Texture* pNormalTexture) {};
	virtual void SetCameraPosition(const float* pData);
	virtual void SetWorldPosition(const float* pData);
	virtual void SetMatrix(const float* m);

	virtual void ChangeSampler(ID3D11Device* m_pDevice, D3D11_FILTER filter);

	ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }


protected:

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};

	ID3DX11EffectMatrixVariable* m_pWorldMatrix{};
	ID3DX11Effect* m_pCurrentEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};


	ID3DX11EffectSamplerVariable* m_pSamplerVariable{};
	ID3D11SamplerState* m_pSamplerState{};



	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectVectorVariable* m_pCameraPosition{};
private:


};
