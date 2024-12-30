//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "CPURenderer.h"
#include "Math.h"
#include "Texture.h"
#include "Utils.h"

#include <algorithm>
#include "Mesh.h"
using namespace dae;

SoftwareRenderer::SoftwareRenderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_pTextureDiffuse	= Texture::LoadFromFile(nullptr,"resources/vehicle_diffuse.png");
	m_pTextureGloss		= Texture::LoadFromFile(nullptr,"resources/vehicle_gloss.png");
	m_pTextureNormal	= Texture::LoadFromFile(nullptr,"resources/vehicle_normal.png");
	m_pTextureSpecular	= Texture::LoadFromFile(nullptr,"resources/vehicle_specular.png");

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-50.f });

	m_Camera.aspectRatio = (float)m_Width / (float)m_Height;

	m_pDepthBufferPixels = new float[m_Width * m_Height];
	std::fill(m_pDepthBufferPixels, m_pDepthBufferPixels + (m_Width * m_Height), FLT_MAX);

	mesh.primitiveTopology = PrimitiveTopology::TriangleList;
	mesh.worldMatrix *= Matrix::CreateRotationY(PI / 2);
	Utils::ParseOBJ("resources/vehicle.obj", mesh.vertices, mesh.indices);


}

SoftwareRenderer::~SoftwareRenderer()
{
	delete[] m_pDepthBufferPixels;
	delete m_pTextureDiffuse;
	delete m_pTextureGloss;
	delete m_pTextureNormal;
	delete m_pTextureSpecular;

}

void SoftwareRenderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
	if (m_bRotate)
	{
		mesh.worldMatrix = Matrix::CreateRotationY(PI * pTimer->GetTotal() / 25);
	}
}

void SoftwareRenderer::Render()
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	std::fill(m_pDepthBufferPixels, m_pDepthBufferPixels + (m_Width * m_Height), FLT_MAX);
	SDL_FillRect(m_pBackBuffer, &m_pBackBuffer->clip_rect, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	VertexTransformationFunction(mesh);

	const int loopStep = mesh.primitiveTopology == PrimitiveTopology::TriangleList ? 3 : 1;
	const int sizeReduction = mesh.primitiveTopology == PrimitiveTopology::TriangleList ? 0 : 2;

	for (int index{}; index < mesh.indices.size() - sizeReduction; index += loopStep)
	{

		Vertex_Out V0 = mesh.vertices_out[mesh.indices[0 + index]];
		Vertex_Out V1 = mesh.vertices_out[mesh.indices[1 + index]];
		Vertex_Out V2 = mesh.vertices_out[mesh.indices[2 + index]];

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

		if (mesh.primitiveTopology == PrimitiveTopology::TriangleStrip)
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

void SoftwareRenderer::VertexTransformationFunction(CPUMesh& mesh) const
{
	//Todo > W1 Projection Stage
	const Matrix WorldViewProjectionMatrix = mesh.worldMatrix * m_Camera.GetViewMatrix() * m_Camera.GetProjectionMatrix();

	mesh.vertices_out.clear();
	mesh.vertices_out.reserve(mesh.vertices.size());

	for (const Vertex& vrtx : mesh.vertices )
	{
		
		Vertex_Out vrtx_temp{ 
			Vector4{vrtx.position.x,vrtx.position.y,vrtx.position.z,0},
			ColorRGB{vrtx.color.x,vrtx.color.y,vrtx.color.z},
			vrtx.uv,
			mesh.worldMatrix.TransformVector(vrtx.normal),
			mesh.worldMatrix.TransformVector(vrtx.tangent),
			vrtx.viewDirection
		};

		vrtx_temp.position = WorldViewProjectionMatrix.TransformPoint(vrtx_temp.position);

		vrtx_temp.position.x /= vrtx_temp.position.w;
		vrtx_temp.position.y /= vrtx_temp.position.w;
		vrtx_temp.position.z /= vrtx_temp.position.w;


		mesh.vertices_out.emplace_back(vrtx_temp);
	}


}

float dae::SoftwareRenderer::Remap(float value, float istart, float istop, float ostart, float ostop)
{
	return (value - istart) / (istop - istart) * (ostop - ostart) + ostart;
}

bool SoftwareRenderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

void SoftwareRenderer::PixelShading(const Vertex_Out& v)
{
	ColorRGB finalColor{};
	ColorRGB diffuse{ m_pTextureDiffuse->Sample(v.uv) };
	ColorRGB gloss{ m_pTextureGloss->Sample(v.uv)};
	ColorRGB normal{m_pTextureNormal->Sample(v.uv)};
	ColorRGB specular{ m_pTextureSpecular->Sample(v.uv)};
	ColorRGB lambertDiffuse{};

	const Vector3 binormal = Vector3::Cross(v.normal, v.tangent);
	const Matrix tangentSpaceAXis = Matrix{ v.tangent,binormal,v.normal,Vector3::Zero };

	Vector3 sampledNorm = Vector3{ normal.r,normal.g,normal.b };

	sampledNorm = 2.f * sampledNorm - Vector3{1.f,1.f,1.f};
	sampledNorm = tangentSpaceAXis.TransformVector(sampledNorm);
	 
	const Vector3 viewDirection = (m_Camera.origin - v.position).Normalized();

	if (!m_bUseNormalMap)
	{
		sampledNorm = v.normal;
	}

	const Vector3 lightDir = { .577f,-.577f,.577f };
	const float cosA = std::max(0.f,Vector3::Dot(-lightDir, sampledNorm));

	if (m_bUseDepth)
	{
		const float remapedVal = Remap(std::clamp(v.position.z, .888f, 1.f), .888f, 1.f, .0f, 1.f);
		finalColor = { remapedVal,remapedVal ,remapedVal };
	}
	else
	{

		const Vector3 reflect = lightDir - 2 * Vector3::Dot(sampledNorm, lightDir) * sampledNorm;
		const float phongCosA = std::max(0.f,Vector3::Dot(viewDirection, reflect.Normalized()));
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
		}
	}

	finalColor.MaxToOne();

	m_pBackBufferPixels[(int)v.position.x + ((int)v.position.y * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

void dae::SoftwareRenderer::NdcToScreenSpace(Vertex_Out& v)
{
	v.position.x = ((v.position.x + 1) * 0.5f) * m_Width;
	v.position.y = ((1 - v.position.y) * 0.5f) * m_Height;
}
