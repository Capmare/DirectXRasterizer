#pragma once
#include <pch.h>
#include <Effect.h>



struct Vertex_PosCol {
	Vector3 position;
	ColorRGB color;
};

class Mesh
{
	Mesh(const ID3D11DeviceContext* device);
	~Mesh();
	
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

public:
	void Render();
protected:
private:
	std::vector<Vertex_PosCol> vertices{

	};

	std::vector<Vertex_PosCol> indices{0, 1, 2};

	ID3D11DeviceContext* m_pDeviceContext;
	Effect* m_pEffect{};
	
};

Mesh::Mesh(const ID3D11DeviceContext* pDevice)
	: m_pDeviceContext{pDevice}
{
	m_pEffect = new Effect(pDevice, "resources/PosCol3D.fx");

	


}

void Mesh::Render()
{
	// set primitive topology
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// set input layout
	//m_pDeviceContext->IASetInputLayout(m_pEffect->) // what is getinputlayout???

	// set vertex buffer
	constexpr UINT stride = sizeof(Vertex_PosCol);
	constexpr UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set index buffer
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect
}
