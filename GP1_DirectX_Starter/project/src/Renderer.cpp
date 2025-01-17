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

		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		m_pCamera.Initialize(60.f, Vector3{ 0.f,0.f,-50.f });

		m_pCamera.aspectRatio = (float)m_Width / (float)m_Height;
		
		//Create Buffers
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

		m_pDepthBufferPixels = new float[m_Width * m_Height];
		std::fill(m_pDepthBufferPixels, m_pDepthBufferPixels + (m_Width * m_Height), FLT_MAX);

		Utils::ParseOBJ("resources/vehicle.obj", vertices, indices);
		Utils::ParseOBJ("resources/fireFX.obj", fireVertices, fireIndices);

		mesh = new Mesh(
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

		m_pTextureDiffuse = Texture::LoadFromFile(m_pDevice, "resources/vehicle_diffuse.png");
		m_pTextureSpecular = Texture::LoadFromFile(m_pDevice, "resources/vehicle_specular.png");
		m_pTextureGloss = Texture::LoadFromFile(m_pDevice, "resources/vehicle_gloss.png");
		m_pTextureNormal = Texture::LoadFromFile(m_pDevice, "resources/vehicle_normal.png");

		

		mesh->GetEffect()->SetDiffuseMap(m_pTextureDiffuse);
		mesh->GetEffect()->SetGlossMap(m_pTextureGloss);
		mesh->GetEffect()->SetSpecularMap(m_pTextureSpecular);
		mesh->GetEffect()->SetNormalMap(m_pTextureNormal);

		FireDiffuse = Texture::LoadFromFile(m_pDevice, "resources/fireFX_diffuse.png");

		fireMesh->GetEffect()->SetDiffuseMap(FireDiffuse);

		mesh->primitiveTopology = PrimitiveTopology::TriangleList;
		mesh->m_Worldmatrix *= Matrix::CreateRotationY(PI);
		fireMesh->m_Worldmatrix *= Matrix::CreateRotationY(PI);

	}

	DirectXRenderer::~DirectXRenderer()
	{
		// order of release matters, has to be inverse of initialization

		delete[] m_pDepthBufferPixels;

	
		if (FireDiffuse)
		{
			delete FireDiffuse;
		}
		if (m_pTextureDiffuse)
		{
			delete m_pTextureDiffuse;
		}
		if (m_pTextureSpecular)
		{
			delete m_pTextureSpecular;
		}
		if (m_pTextureGloss)
		{
			delete m_pTextureGloss;
		}
		if (m_pTextureNormal)
		{
			delete m_pTextureNormal;
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
		if (mesh)
		{
			delete mesh;
		}
	}

	void DirectXRenderer::Update(const Timer* pTimer)
	{
		m_pCamera.Update(pTimer);

		mesh->m_Worldmatrix = Matrix::CreateRotationY(PI * currentRotTime / 25);
		fireMesh->m_Worldmatrix = Matrix::CreateRotationY(PI * currentRotTime / 25);

		if (m_bRotate)
		{
			currentRotTime += pTimer->GetElapsed();			
		}
		
		
	}


	void DirectXRenderer::VertexTransformationFunction(Mesh* mesh) const
	{
		//Todo > W1 Projection Stage
		const Matrix WorldViewProjectionMatrix = mesh->m_Worldmatrix * m_pCamera.GetViewMatrix() * m_pCamera.GetProjectionMatrix();

		mesh->m_vertexDataOut.clear();
		mesh->m_vertexDataOut.reserve(mesh->m_vertexData.size());

		for (const Vertex& vrtx : mesh->m_vertexData)
		{

			Vertex_Out vrtx_temp{
				Vector4{vrtx.position.x,vrtx.position.y,vrtx.position.z,0},
				ColorRGB{vrtx.color.x,vrtx.color.y,vrtx.color.z},
				vrtx.uv,
				mesh->m_Worldmatrix.TransformVector(vrtx.normal),
				mesh->m_Worldmatrix.TransformVector(vrtx.tangent),
				vrtx.viewDirection
			};

			vrtx_temp.position = WorldViewProjectionMatrix.TransformPoint(vrtx_temp.position);

			vrtx_temp.position.x /= vrtx_temp.position.w;
			vrtx_temp.position.y /= vrtx_temp.position.w;
			vrtx_temp.position.z /= vrtx_temp.position.w;


			mesh->m_vertexDataOut.emplace_back(vrtx_temp);
		}
	}

	float DirectXRenderer::Remap(float value, float istart, float istop, float ostart, float ostop)
	{
		return (value - istart) / (istop - istart) * (ostop - ostart) + ostart;
	}

	void DirectXRenderer::PixelShading(const Vertex_Out& v)
	{
		ColorRGB finalColor{};
		ColorRGB diffuse{ m_pTextureDiffuse->Sample(v.uv) };
		ColorRGB gloss{ m_pTextureGloss->Sample(v.uv) };
		ColorRGB normal{ m_pTextureNormal->Sample(v.uv) };
		ColorRGB specular{ m_pTextureSpecular->Sample(v.uv) };
		ColorRGB lambertDiffuse{};

		const Vector3 binormal = Vector3::Cross(v.normal, v.tangent);
		const Matrix tangentSpaceAXis = Matrix{ v.tangent,binormal,v.normal,Vector3::Zero };

		Vector3 sampledNorm = Vector3{ normal.r,normal.g,normal.b };

		sampledNorm = 2.f * sampledNorm - Vector3{ 1.f,1.f,1.f };
		sampledNorm = tangentSpaceAXis.TransformVector(sampledNorm);

		const Vector3 viewDirection = (m_pCamera.origin - v.position).Normalized();

		if (!m_bUseNormalMap)
		{
			sampledNorm = v.normal;
		}

		const Vector3 lightDir = { .577f,-.577f,.577f };
		const float cosA = std::max(0.f, Vector3::Dot(-lightDir, sampledNorm));

		if (m_bUseDepth)
		{
			const float remapedVal = Remap(std::clamp(v.position.z, .888f, 1.f), .888f, 1.f, .0f, 1.f);
			finalColor = { remapedVal,remapedVal ,remapedVal };
		}
		else
		{

			const Vector3 reflect = lightDir - 2 * Vector3::Dot(sampledNorm, lightDir) * sampledNorm;
			const float phongCosA = std::max(0.f, Vector3::Dot(viewDirection, reflect.Normalized()));
			const ColorRGB PhonSpec = specular * pow(phongCosA, gloss.r * m_Shininess);
			lambertDiffuse = diffuse * 7.f / PI;

			switch (m_CurrentLightingMode)
			{
			case LightingMode::Combined:
				finalColor = lambertDiffuse + PhonSpec + ColorRGB{ 0.025f,0.025f ,0.025f };
				finalColor *= cosA;
				break;
			case LightingMode::Diffuse:
				finalColor = lambertDiffuse;
				break;
			case LightingMode::OA:
				finalColor = colors::White;
				finalColor *= cosA;
				break;
			case LightingMode::Specular:
				finalColor = PhonSpec + ColorRGB{ 0.025f,0.025f ,0.025f };
				break;
			default:
				break;
			}
		}

		finalColor.MaxToOne();

		m_pBackBufferPixels[(int)v.position.x + ((int)v.position.y * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
			static_cast<uint8_t>(finalColor.r * 255),
			static_cast<uint8_t>(finalColor.g * 255),
			static_cast<uint8_t>(finalColor.b * 255));
	}


	void DirectXRenderer::RenderOnCPU()
	{
		//@START
	//Lock BackBuffer
		SDL_LockSurface(m_pBackBuffer);

		std::fill(m_pDepthBufferPixels, m_pDepthBufferPixels + (m_Width * m_Height), FLT_MAX);

		if (m_bIsUniformColor)
		{
			SDL_FillRect(m_pBackBuffer, &m_pBackBuffer->clip_rect, SDL_MapRGB(m_pBackBuffer->format, 0, 0, 0));

		}
		else
		{
			SDL_FillRect(m_pBackBuffer, &m_pBackBuffer->clip_rect, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

		}

		VertexTransformationFunction(mesh);

		const int loopStep = mesh->primitiveTopology == PrimitiveTopology::TriangleList ? 3 : 1;
		const int sizeReduction = mesh->primitiveTopology == PrimitiveTopology::TriangleList ? 0 : 2;

		for (int index{}; index < mesh->m_indexData.size() - sizeReduction; index += loopStep)
		{

			Vertex_Out V0 =mesh->m_vertexDataOut[mesh->m_indexData[0 + index]];
			Vertex_Out V1 =mesh->m_vertexDataOut[mesh->m_indexData[1 + index]];
			Vertex_Out V2 =mesh->m_vertexDataOut[mesh->m_indexData[2 + index]];

			if (
				V0.position.x < -1 || V0.position.x > 1 ||
				V0.position.y < -1 || V0.position.y > 1 ||
				V0.position.w < 0 ||

				V1.position.x < -1 || V1.position.x > 1 ||
				V1.position.y < -1 || V1.position.y > 1 ||
				V1.position.w < 0 ||

				V2.position.x < -1 || V2.position.x > 1 ||
				V2.position.y < -1 || V2.position.y > 1 ||
				V2.position.w < 0
				)

			{
				continue;
			}

			NdcToScreenSpace(V0);
			NdcToScreenSpace(V1);
			NdcToScreenSpace(V2);

			if (mesh->primitiveTopology == PrimitiveTopology::TriangleStrip)
			{
				if (index % 2 != 0)  std::swap(V1, V2);
			}

			const int BiggestXVal = std::min(std::max({ V0.position.x,V1.position.x,V2.position.x }), (float)m_Width);
			const int BiggestYVal = std::min(std::max({ V0.position.y,V1.position.y,V2.position.y }), (float)m_Height);

			const int SmallestXVal = std::max(std::min({ V0.position.x,V1.position.x,V2.position.x }), 0.f);
			const int SmallestYVal = std::max(std::min({ V0.position.y,V1.position.y,V2.position.y }), 0.f);

			//RENDER LOGIC
			for (int px{ SmallestXVal }; px <= BiggestXVal; ++px)
			{
				for (int py{ SmallestYVal }; py <= BiggestYVal; ++py)
				{

					if (m_bUseBoundingBox)
					{
						Vertex_Out BoundingBoxPixel{};

						BoundingBoxPixel.position = Vector4{ (float)px,(float)py,1,1 };
						BoundingBoxPixel.color = ColorRGB{ 1,1,1 };

						m_pBackBufferPixels[(int)px + ((int)py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(BoundingBoxPixel.color.r * 255),
							static_cast<uint8_t>(BoundingBoxPixel.color.g * 255),
							static_cast<uint8_t>(BoundingBoxPixel.color.b * 255));
						continue;
					}
					



					const Vector2 P{ px + .5f, py + .5f };


					const Vector2 e0{ V2.position.GetXY() - V1.position.GetXY() };
					const Vector2 p0{ P - V1.position.GetXY() };
					float W0 = Vector2::Cross(e0, p0);
					if (W0 < 0) continue;

					const Vector2 e1 = { V0.position.GetXY() - V2.position.GetXY() };
					const Vector2 p1 = { P - V2.position.GetXY() };
					float W1 = Vector2::Cross(e1, p1);
					if (W1 < 0) continue;

					const Vector2 e2 = { V1.position.GetXY() - V0.position.GetXY() };
					const Vector2 p2 = { P - V0.position.GetXY() };
					float W2 = Vector2::Cross(e2, p2);
					if (W2 < 0) continue;

					const float TotalArea = W0 + W1 + W2;
					if (TotalArea <= 0) continue;

					W0 /= TotalArea;
					W1 /= TotalArea;
					W2 /= TotalArea;

					const Vector3 PInsideTriangle = W0 * V0.position + W1 * V1.position + W2 * V2.position;

					const float wInterp =
						1 /
						(
							(1 / (V0.position.w) * W0) +
							(1 / (V1.position.w) * W1) +
							(1 / (V2.position.w) * W2)
							);


					const float zInterp =
						1 /
						(
							(1 / (V0.position.z) * W0) +
							(1 / (V1.position.z) * W1) +
							(1 / (V2.position.z) * W2)
							);

					if (zInterp > m_pDepthBufferPixels[px + py * m_Width] || zInterp > 1 || zInterp < 0) continue;



					m_pDepthBufferPixels[px + py * m_Width] = zInterp;


					const Vertex_Out finalPixel
					{
						Vector4{ (float)px,(float)py,zInterp,wInterp },
						V0.color / V0.position.w * W0 + V1.color / V1.position.w * W1 + V2.color / V2.position.w * W2,
						Vector2(V0.uv / V0.position.w * W0 + V1.uv / V1.position.w * W1 + V2.uv / V2.position.w * W2) * wInterp,
						V0.normal * W0 + V1.normal * W1 + V2.normal * W2
					};


					PixelShading(finalPixel);


				}
			}
		}

		//@END
	//Update SDL Surface
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);

	}

	void DirectXRenderer::Render() const
	{


		if (!m_IsInitialized)
			return;

		//// clear back buffer
		float color[4] = { 0.f,0.f,0.3f,1.f };

		if (m_bIsUniformColor)
		{
			color[0] = 0.f;
			color[1] = 0.f;
			color[2] = 0.f;
		}
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);

		mesh->GetEffect()->SetCameraPosition(reinterpret_cast<const float*>(&m_pCamera.origin));
		fireMesh->GetEffect()->SetCameraPosition(reinterpret_cast<const float*>(&m_pCamera.origin));

		mesh->Render(m_pDeviceContext, mesh->m_Worldmatrix * m_pCamera.GetViewMatrix() * m_pCamera.GetProjectionMatrix());
		if (m_bShowFire)
		{
			fireMesh->Render(m_pDeviceContext, fireMesh->m_Worldmatrix * m_pCamera.GetViewMatrix() * m_pCamera.GetProjectionMatrix());
		}

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
		mesh->GetEffect()->ChangeSampler(m_pDevice,currentFilter);
		if (samplerCount == 2)
		{
			samplerCount = 0; 
			return;
		}
		++samplerCount;
	}

	void DirectXRenderer::ChangeDirectXCullingMode()
	{
		D3D11_CULL_MODE newCullMode{};

		switch (mesh->m_CurrentCullingMode)
		{
		default:
			break;
		case CulingMode::None:
			newCullMode = D3D11_CULL_BACK;
			mesh->m_CurrentCullingMode = CulingMode::Back;
			break;
		case CulingMode::Back:
			newCullMode = D3D11_CULL_FRONT;
			mesh->m_CurrentCullingMode = CulingMode::Front;
			break;
		case CulingMode::Front:
			newCullMode = D3D11_CULL_NONE	;
			mesh->m_CurrentCullingMode = CulingMode::None;
			break;
		}

		mesh->GetEffect()->ChangeDirectXCullingMode(m_pDevice,newCullMode);


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


		return S_OK;
	}

	void DirectXRenderer::NdcToScreenSpace(Vertex_Out& v)
	{
		v.position.x = ((v.position.x + 1) * 0.5f) * m_Width;
		v.position.y = ((1 - v.position.y) * 0.5f) * m_Height;
	}

}
