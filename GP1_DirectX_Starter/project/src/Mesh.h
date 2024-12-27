#pragma once
#include "pch.h"
#include "Math.h"

class Effect;


struct Vertex {
	Vector3	position;
	Vector3	color;
	Vector2	uv;
	Vector3 normal; 
	Vector3 tangent; 
};

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device* pDevice, std::vector<Vertex> vertexData, std::vector<uint32_t> indexData);
	~Mesh();
	
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContex, const Matrix& viewProj);
	Effect* GetEffect() const { return m_pEffect; }

	Matrix m_Worldmatrix
	{
		Vector4{1,0,0,0},
		Vector4{0,1,0,0},
		Vector4{0,0,1,0},
		Vector4{0,0,0,1}
	};
private:
	Effect* m_pEffect;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;




	uint32_t m_NumIndeces;
};
