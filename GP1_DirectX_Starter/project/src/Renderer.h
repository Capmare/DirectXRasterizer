#pragma once
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory1* m_pDxgiFactory{};
		IDXGISwapChain* m_SwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		//DIRECTX
		HRESULT InitializeDirectX();
		//...

		
		std::vector<Vertex> vertices{
			{XMFLOAT3{.0f,.5f,.5f},		XMFLOAT3{1.f,0.f,0.f}},
			{XMFLOAT3{.5f,-.5f,.5f},	XMFLOAT3{0.f,0.f,1.f} },
			{XMFLOAT3{-.5f,-.5f,.5f},	XMFLOAT3{ 0.f,1.f,0.f }}
		};

		std::vector<uint32_t> indeces{ 0,1,2 };


		Mesh* triangleMesh{};


		const FLOAT BackgroundColor[4] = { 0.f,0.f,0.f,0.f };

	};
}
