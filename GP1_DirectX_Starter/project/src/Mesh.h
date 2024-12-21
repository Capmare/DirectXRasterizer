#pragma once
#include "pch.h"

class Effect;


struct Vertex {
	XMFLOAT3  position;
	XMFLOAT3  color;
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

	void Render(ID3D11DeviceContext*  pDeviceContex);

private:
	Effect* m_pEffect;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	uint32_t m_NumIndeces;
};
