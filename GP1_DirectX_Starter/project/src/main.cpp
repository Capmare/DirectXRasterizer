#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void InvertBool(bool& b) { b = !b; };

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


	printf(COLOR_GREEN "[Key Bindings - SHARED]\n" COLOR_RESET
		COLOR_YELLOW "[F1] Toggle Rasterizer Mode (HARDWARE/SOFTWARE)\n" COLOR_RESET
		COLOR_YELLOW "[F2] Toggle Vehicle Rotation (ON/OFF)\n" COLOR_RESET
		COLOR_YELLOW "[F9] Cycle CullMode (BACK/FRONT/NONE)\n" COLOR_RESET
		COLOR_YELLOW "[F10] Toggle Uniform ClearColor (ON/OFF)\n" COLOR_RESET
		COLOR_YELLOW "[F11] Toggle Print FPS (ON/OFF)\n\n" COLOR_RESET
		COLOR_GREEN "[Key Bindings - HARDWARE]\n" COLOR_RESET
		COLOR_CYAN "[F3] Toggle FireFX (ON/OFF)\n" COLOR_RESET
		COLOR_CYAN "[F4] Cycle Sampler State (POINT/LINEAR/ANISOTROPIC)\n\n" COLOR_RESET
		COLOR_GREEN "[Key Bindings - SOFTWARE]\n" COLOR_RESET
		COLOR_MAGENTA "[F5] Cycle Shading Mode (COMBINED/OBSERVED_AREA/DIFFUSE/SPECULAR)\n" COLOR_RESET
		COLOR_MAGENTA "[F6] Toggle NormalMap (ON/OFF)\n" COLOR_RESET
		COLOR_MAGENTA "[F7] Toggle DepthBuffer Visualization (ON/OFF)\n" COLOR_RESET
		COLOR_MAGENTA "[F8] Toggle BoundingBox Visualization (ON/OFF)\n" COLOR_RESET);



	auto pRenderer = new DirectXRenderer(pWindow);
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool bIsDirectX{ true };
	bool bShowFps{ false };
	while (isLooping)
	{
		//--------- Get input events ---------
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
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					InvertBool(bIsDirectX);
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleRotate();

				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					if (bIsDirectX)
					{
						pRenderer->ChangeDirectXCullingMode();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->UseUniformColor();

				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					InvertBool(bShowFps);
				}
				if (bIsDirectX)
				{
					if (e.key.keysym.scancode == SDL_SCANCODE_F3)
					{
						pRenderer->ShowFire();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F4)
					{
						pRenderer->ChangeToNextSampler();
					}

				}
				if (!bIsDirectX)
				{
					if (e.key.keysym.scancode == SDL_SCANCODE_F5)
					{
						pRenderer->NextLightingMode();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F6)
					{
						pRenderer->UseNormalMap();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F7)
					{
						pRenderer->UseDepth();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_F8)
					{
						pRenderer->UseBoundingBox();
					}
				}
				

				break;
			default:;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);
		//--------- Render ---------
		if (bIsDirectX)
		{
			pRenderer->Render();
		}
		else
		{
			pRenderer->RenderOnCPU();
		}
	
		//--------- Timer ---------
		pTimer->Update();
		if (bShowFps)
		{
			printTimer += pTimer->GetElapsed();
			if (printTimer >= 1.f)
			{
				printTimer = 0.f;
				std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
			}
		}
		
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}