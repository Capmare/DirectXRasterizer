#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{



	class DirectXRenderer final
	{
	public:
		DirectXRenderer(SDL_Window* pWindow);
		~DirectXRenderer();

		DirectXRenderer(const DirectXRenderer&) = delete;
		DirectXRenderer(DirectXRenderer&&) noexcept = delete;
		DirectXRenderer& operator=(const DirectXRenderer&) = delete;
		DirectXRenderer& operator=(DirectXRenderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		void ChangeToNextSampler();

		void ToggleRotate() const {};
		Camera m_pCamera;


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
			{ { -3.f,  3.f, -2.f}, {1.f,1.f,1.f}, { 0, 0 } },
			{ {  .0f,  3.f, -2.f}, {1.f,1.f,1.f}, { .5, 0 } },
			{ {  3.f,  3.f, -2.f}, {1.f,1.f,1.f}, { 1, 0 } },
			{ { -3.f,  .0f, -2.f}, {1.f,1.f,1.f}, { 0, .5 } },
			{ {  .0f,  .0f, -2.f}, {1.f,1.f,1.f}, { .5, .5 } },
			{ {  3.f,  .0f, -2.f}, {1.f,1.f,1.f}, { 1, .5 } },
			{ { -3.f, -3.f, -2.f}, {1.f,1.f,1.f}, { 0, 1 } },
			{ {  .0f, -3.f, -2.f}, {1.f,1.f,1.f}, { .5, 1 } },
			{ {  3.f, -3.f, -2.f}, {1.f,1.f,1.f}, { 1, 1 } }
		};
		std::vector<uint32_t> indices{
			3, 0, 1, 1, 4, 3, 4, 1, 2,
			2, 5, 4, 6, 3, 4, 4, 7, 6,
			7, 4, 5, 5, 8, 7
		};


		Mesh* triangleMesh{};
		Texture* Diffuse{};
		Texture* Specular{};
		Texture* Gloss{};
		Texture* Normal{};


		int samplerCount{};
	};
}
