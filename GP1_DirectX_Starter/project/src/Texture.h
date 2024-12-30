#pragma once
#include "pch.h"
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;

	

	class Texture
	{
	public:
		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);
		~Texture();

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);
		ColorRGB Sample(const Vector2& uv) const;
		ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }

	private:

		ID3D11Device* m_pDevice{};
		SDL_Surface* m_pSurface{ nullptr };

		ID3D11Texture2D* m_pResource;
		ID3D11ShaderResourceView* m_pSRV;

		uint32_t* m_pSurfacePixels{ nullptr };

	};
}