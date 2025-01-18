#pragma once
#include "pch.h"
#include <SDL_surface.h>
#include <string>
#include <unordered_map>
#include <type_traits>
#include "ColorRGB.h"
#include "Mesh.h"

namespace dae
{
	struct Vector2;

	struct hash_pair {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& p) const {
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ hash2;
		}
	};
	

	class Texture
	{
	public:
		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);
		~Texture();

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);
		void PrecomputeCache();
		ColorRGB Sample(const Vector2& uv) const;
		ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }




	private:


		ID3D11Device* m_pDevice{};

		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pSRV{};

		ColorRGB* cache;

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };

	};
}



