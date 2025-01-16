#pragma once
#include "pch.h"
#include "BaseEffectClass.h"

class VFXEffect : public BaseEffectClass
{
public:
	
	VFXEffect(ID3D11Device* pDevice, const std::wstring& effectAsset);
	~VFXEffect() = default;
	
	VFXEffect(const VFXEffect&) = delete;
	VFXEffect(VFXEffect&&) noexcept = delete;
	VFXEffect& operator=(const VFXEffect&) = delete;
	VFXEffect& operator=(VFXEffect&&) noexcept = delete;
	
protected:
	
private:
};
