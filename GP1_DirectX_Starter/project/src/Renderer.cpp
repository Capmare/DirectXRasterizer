#include "pch.h"
#include "Renderer.h"
#include "Utils.h"
#include "Effect.h"

namespace dae {

	DirectXRenderer::DirectXRenderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";

			m_pCamera.Initialize(60.f, Vector3{ 0.f,0.f,-50.f });
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		m_pCamera.aspectRatio = (float)m_Width / (float)m_Height;
		
		triangleMesh->m_Worldmatrix *= Matrix::CreateRotationY(PI);
		fireMesh->m_Worldmatrix *= Matrix::CreateRotationY(PI);

	}

	DirectXRenderer::~DirectXRenderer()
	{
		// order of release matters, has to be inverse of initialization
		if (FireDiffuse)
		{
			delete FireDiffuse;
		}
		if (Diffuse)
		{
			delete Diffuse;
		}
		if (Specular)
		{
			delete Specular;
		}
		if (Gloss)
		{
			delete Gloss;
		}
		if (Normal)
		{
			delete Normal;
		}
		if (m_pRenderTargetView)
		{
			m_pRenderTargetView->Release();

		}
		if (m_pRenderTargetBuffer)
		{
			m_pRenderTargetBuffer->Release();
		}
		if (m_pDepthStencilView)
		{
			m_pDepthStencilView->Release();
		}
		if (m_pDepthStencilBuffer)
		{
			m_pDepthStencilBuffer->Release();
		}
		if (m_SwapChain)
		{
			m_SwapChain->Release();
		}
		if (m_pDxgiFactory)
		{
			m_pDxgiFactory->Release();
		}
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice)
		{
			m_pDevice->Release();
		}
		if (fireMesh)
		{
			delete fireMesh;
		}
		if (triangleMesh)
		{
			delete triangleMesh;
		}
	}

	void DirectXRenderer::Update(const Timer* pTimer)
	{
		m_pCamera.Update(pTimer);

		triangleMesh->m_Worldmatrix = Matrix::CreateRotationY(PI * currentRotTime / 25);
		fireMesh->m_Worldmatrix = Matrix::CreateRotationY(PI * currentRotTime / 25);

		if (m_bRotate)
		{
			currentRotTime += pTimer->GetElapsed();			
		}
		
		
	}


	void DirectXRenderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//// clear back buffer
		constexpr float color[4] = { 0.f,0.f,0.3f,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);

		triangleMesh->GetEffect()->SetCameraPosition(reinterpret_cast<const float*>(&m_pCamera.origin));
		fireMesh->GetEffect()->SetCameraPosition(reinterpret_cast<const float*>(&m_pCamera.origin));

		triangleMesh->Render(m_pDeviceContext, triangleMesh->m_Worldmatrix * m_pCamera.GetViewMatrix() * m_pCamera.GetProjectionMatrix());
		fireMesh->Render(m_pDeviceContext, fireMesh->m_Worldmatrix * m_pCamera.GetViewMatrix() * m_pCamera.GetProjectionMatrix());

		// switch the back buffer and front buffer
		m_SwapChain->Present(0, 0);
		

	}


	void DirectXRenderer::ChangeToNextSampler()
	{
		D3D11_FILTER currentFilter{};

		if (samplerCount == 0)
		{
			currentFilter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		}
		if (samplerCount == 1)
		{
			
			currentFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		}
		if (samplerCount == 2)
		{
			
			currentFilter = D3D11_FILTER_ANISOTROPIC;

		}
		triangleMesh->GetEffect()->ChangeSampler(m_pDevice,currentFilter);
		if (samplerCount == 2)
		{
			samplerCount = 0; 
			return;
		}
		++samplerCount;
	}

	HRESULT DirectXRenderer::InitializeDirectX()
	{

		// Create Device and DC

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlag, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result)) return result;

		// Create DXGI factory
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&m_pDxgiFactory));
		if (FAILED(result)) return result;
		// create swapchaindesc

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};

		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// handle to the back buffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//create the swap chain
		result = m_pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_SwapChain);
		if (FAILED(result)) return result;

		// create depth stencil buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result)) return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result)) return result;

		// create render target
		result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result)) return result;

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result)) return result;

		// bind RTV and DSV out to merger stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// set viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;

		m_pDeviceContext->RSSetViewports(1, &viewport);


		Utils::ParseOBJ("resources/vehicle.obj", vertices, indices);
		Utils::ParseOBJ("resources/fireFX.obj", fireVertices, fireIndices);

		triangleMesh = new Mesh(
			m_pDevice,
			vertices,
			indices,
			ShaderType::Diffuse
		);

		fireMesh = new Mesh(
			m_pDevice,
			fireVertices,
			fireIndices, 
			ShaderType::VFX
		);

		Diffuse = Texture::LoadFromFile(m_pDevice, "resources/vehicle_diffuse.png");
		Specular = Texture::LoadFromFile(m_pDevice, "resources/vehicle_specular.png");
		Gloss = Texture::LoadFromFile(m_pDevice, "resources/vehicle_gloss.png");
		Normal = Texture::LoadFromFile(m_pDevice, "resources/vehicle_normal.png");

		triangleMesh->GetEffect()->SetDiffuseMap(Diffuse);
		triangleMesh->GetEffect()->SetGlossMap(Gloss);
		triangleMesh->GetEffect()->SetSpecularMap(Specular);
		triangleMesh->GetEffect()->SetNormalMap(Normal);

		FireDiffuse = Texture::LoadFromFile(m_pDevice, "resources/fireFX_diffuse.png");
		
		fireMesh->GetEffect()->SetDiffuseMap(FireDiffuse);


		return S_OK;
	}

}
