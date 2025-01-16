#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct CPUMesh;
	class Timer;
	class Scene;

	


	class SoftwareRenderer final
	{
	public:
		SoftwareRenderer(SDL_Window* pWindow);
		~SoftwareRenderer();

		SoftwareRenderer(const SoftwareRenderer&) = delete;
		SoftwareRenderer(SoftwareRenderer&&) noexcept = delete;
		SoftwareRenderer& operator=(const SoftwareRenderer&) = delete;
		SoftwareRenderer& operator=(SoftwareRenderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

		bool SaveBufferToImage() const;

		void VertexTransformationFunction(CPUMesh& mesh) const;

		float Remap(float value, float istart, float istop, float ostart, float ostop);

		void ToggleDepth() { m_bUseDepth = !m_bUseDepth; }
		void ToggleRotate() { m_bRotate = !m_bRotate; }
		void ToggleNormalMap() { m_bUseNormalMap = !m_bUseNormalMap; }

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		Camera m_Camera{};
		float currentRotTime{};
		bool m_bRotate{ true };

	private:

		void PixelShading(const Vertex_Out& v);
		void NdcToScreenSpace(Vertex_Out& v);

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		CPUMesh mesh{};

		Texture* m_pTextureDiffuse{};
		Texture* m_pTextureGloss{};
		Texture* m_pTextureNormal{};
		Texture* m_pTextureSpecular{};


		float* m_pDepthBufferPixels{};
		const float m_Shininess{ 25.f };
		int m_Width{};
		int m_Height{};
		bool m_bUseDepth{ false };
		bool m_bUseNormalMap{ true };
	};
}
