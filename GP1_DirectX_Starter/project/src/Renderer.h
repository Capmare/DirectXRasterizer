#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "ThreadPool.h"

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
		void VertexTransformationFunction(Mesh* mesh) const;
		float Remap(float value, float istart, float istop, float ostart, float ostop);
		void PixelShading(const Vertex_Out& v);
		void RenderOnCPU();
		void Render() const;

		
		void IsDirectX(const bool& bIsTrue) { m_pCamera.SetIsDirectX(bIsTrue); };
		void ChangeToNextSampler();

		void ToggleRotate() { m_bRotate = !m_bRotate; };
		void ShowFire() { m_bShowFire = !m_bShowFire; };
		void NextLightingMode(){

			switch (m_CurrentLightingMode)
			{
			default:
				break;
			case LightingMode::OA:
				m_CurrentLightingMode = LightingMode::Diffuse;
				printf("\nDiffuse rendering\n");
				break;
			case LightingMode::Diffuse:
				m_CurrentLightingMode = LightingMode::Specular;
				printf("\nSpecular rendering\n");
				break;
			case LightingMode::Specular:
				m_CurrentLightingMode = LightingMode::Combined;
				printf("\nCombined rendering\n");
				break;
			case LightingMode::Combined:
				m_CurrentLightingMode = LightingMode::OA;
				printf("\nOA rendering\n");
				break;
			}

			
		}
		void UseNormalMap() { m_bUseNormalMap = !m_bUseNormalMap; }
		void UseDepth() { m_bUseDepth = !m_bUseDepth; }
		void UseUniformColor() { m_bIsUniformColor = !m_bIsUniformColor; }
		void UseBoundingBox() { m_bUseBoundingBox = !m_bUseBoundingBox; }
		void ChangeDirectXCullingMode();
		D3D11_CULL_MODE ChangeCulling();

	private:
		SDL_Window* m_pWindow{};

		Camera m_pCamera;



		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory1* m_pDxgiFactory{};
		IDXGISwapChain* m_SwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};


		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		float* m_pDepthBufferPixels{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		//DIRECTX
		HRESULT InitializeDirectX();
		void NdcToScreenSpace(Vertex_Out& v);
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

		std::vector<Vertex> fireVertices{};
		std::vector<uint32_t> fireIndices{};


		Mesh* mesh{};
		Mesh* fireMesh{};

		Texture* m_pTextureDiffuse{};
		Texture* m_pTextureSpecular{};
		Texture* m_pTextureGloss{}; 
		Texture* m_pTextureNormal{}; 

		Texture* FireDiffuse{};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };

		const float m_Shininess{ 25.f };
		float currentRotTime{};
		int samplerCount{};
		bool m_bUseNormalMap{true};
		bool m_bUseDepth{ false };
		bool m_bRotate{ true };
		bool m_bShowFire{ true };
		bool m_bIsUniformColor{ false };
		bool m_bUseBoundingBox{ false };
	};
}
