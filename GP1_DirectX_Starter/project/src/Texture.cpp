#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(ID3D11Device* pDevice, SDL_Surface* pSurface)
		: m_pSurface{ pSurface }, m_pDevice{ pDevice }, m_pSurfacePixels{ (uint32_t*)pSurface->pixels}
	{
		cache = new ColorRGB[m_pSurface->w * m_pSurface->h];

		if (pDevice)
		{
			// initialize and create texture resource
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11_TEXTURE2D_DESC desc{};
			desc.Width = m_pSurface->w;
			desc.Height = m_pSurface->h;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData{};
			initData.pSysMem = m_pSurface->pixels;
			initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
			initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

			HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
			if (FAILED(hr)) return;

			// create shader resource
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
			SRVDesc.Format = format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;

			hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
			if (!FAILED(hr)) return;

			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
		
	}

	Texture::~Texture()
	{
		if (cache)
		{
			delete cache;
		}
		if (m_pDevice)
		{
			if (m_pSRV)
			{
				m_pSRV->Release();
			}
			if (m_pResource)
			{
				m_pResource->Release();
			}
		}
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
		
	}

	Texture* Texture::LoadFromFile(ID3D11Device* pDevice, const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)


		return new Texture{ pDevice,IMG_Load(path.c_str()) };
	}

	void Texture::PrecomputeCache()
	{
		Uint8 r, g, b;
		for (int y = 0; y < m_pSurface->h; ++y) {
			for (int x = 0; x < m_pSurface->w; ++x) {
				SDL_GetRGB(m_pSurfacePixels[x + y * m_pSurface->w], m_pSurface->format, &r, &g, &b);
				ColorRGB color = { r / 255.f, g / 255.f, b / 255.f };
				cache[x + y * m_pSurface->w] = color;
			}
		}
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		int x = int(uv.x * m_pSurface->w);
		int y = int(uv.y * m_pSurface->h);
		return cache[x + y * m_pSurface->w];
	}

}