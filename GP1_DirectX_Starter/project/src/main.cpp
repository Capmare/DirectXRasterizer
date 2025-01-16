#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"
#include "CPURenderer.h"


using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - ***Insert Name/Class***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	bool bIsDirectX{ true };
	auto pRenderer = new DirectXRenderer(pWindow);
	auto pCPURenderer = new SoftwareRenderer(pWindow);
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		if (bIsDirectX)
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					isLooping = false;
					break;
				case SDL_KEYUP:
					//Test for a key
					if (e.key.keysym.scancode == SDL_SCANCODE_K)
					{
						pCPURenderer->m_Camera.origin = pRenderer->m_pCamera.origin;
						pCPURenderer->m_Camera.forward = pRenderer->m_pCamera.forward;
						pCPURenderer->currentRotTime = pRenderer->currentRotTime;
						pCPURenderer->m_bRotate = pRenderer->m_bRotate;

						bIsDirectX = false;
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F2)
					{
						pRenderer->ChangeToNextSampler();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F5)
					{
						pRenderer->ToggleRotate();


					}

					break;
				default:;
				}
			}
		}
		else
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					isLooping = false;
					break;
				case SDL_KEYUP:
					if (e.key.keysym.scancode == SDL_SCANCODE_K)
					{
						pRenderer->m_pCamera.origin = pCPURenderer->m_Camera.origin;
						pRenderer->m_pCamera.forward  = pCPURenderer->m_Camera.forward;
						pRenderer->currentRotTime = pCPURenderer->currentRotTime;
						pRenderer->m_bRotate = pCPURenderer->m_bRotate;
						bIsDirectX = true;

					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F4)
					{
						pCPURenderer->ToggleDepth();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F5)
					{
						pCPURenderer->ToggleRotate();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F6)
					{
						pCPURenderer->ToggleNormalMap();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F7)
					{
						if (pCPURenderer->m_CurrentLightingMode == LightingMode::Combined)
						{
							pCPURenderer->m_CurrentLightingMode = LightingMode::OA;
						}
						else
						{
							pCPURenderer->m_CurrentLightingMode = static_cast<LightingMode>((int)pCPURenderer->m_CurrentLightingMode + 1);
						}
					}

					break;
				}
			}
		}
		

		if (bIsDirectX)
		{
			//--------- Update ---------
			pRenderer->Update(pTimer);
			//--------- Render ---------
			pRenderer->Render();

		}
		else
		{
			//--------- Update ---------
			pCPURenderer->Update(pTimer);
			//--------- Render ---------
			pCPURenderer->Render();

		}
		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;
	delete pCPURenderer;

	ShutDown(pWindow);
	return 0;
}