#pragma once
#include "pch.h"
#include "Math.h"



class BaseEffectClass;

enum class CulingMode
{
	None,
	Back,
	Front
};
enum class LightingMode
{
	OA,
	Diffuse,
	Specular,
	Combined
};

struct Vertex {
	Vector3	position;
	Vector3	color;
	Vector2	uv;
	Vector3 normal;
	Vector3 tangent;

	Vector3 viewDirection{}; // only for cpu
};

struct Vertex_Out
{
	Vector4 position{};
	ColorRGB color{ colors::White };
	Vector2 uv{};
	Vector3 normal{};
	Vector3 tangent{};
	Vector3 viewDirection{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

enum class ShaderType{
	Diffuse,
	VFX
};

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device* pDevice, std::vector<Vertex> vertexData, std::vector<uint32_t> indexData, ShaderType shaderType);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContex, const Matrix& viewProj);
	BaseEffectClass* GetEffect() const { return m_pEffect; }

	Matrix m_Worldmatrix
	{
		Vector4{1,0,0,0},
		Vector4{0,1,0,0},
		Vector4{0,0,1,0},
		Vector4{0,0,0,1}
	};

	PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

	ShaderType shader{};

	std::vector<Vertex> m_vertexData{};
	std::vector<Vertex_Out> m_vertexDataOut{};

	std::vector<uint32_t> m_indexData{};

	CulingMode m_CurrentCullingMode{ CulingMode::None };
private:
	BaseEffectClass* m_pEffect{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};



	uint32_t m_NumIndeces{};
};
