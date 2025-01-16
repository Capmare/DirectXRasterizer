#include "Mesh.h"
#include "Effect.h"
#include "VFXEffect.h"
#include "BaseEffectClass.h"

Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex> vertexData, std::vector<uint32_t> indexData, ShaderType shaderType) : shader{shaderType}
{

	switch (shader)
	{
	default:
		break;
	case ShaderType::Diffuse:
		m_pEffect = reinterpret_cast<BaseEffectClass*>(new Effect(pDevice, L"resources/PosCol3d.fx"));
		break;
	case ShaderType::VFX:
		m_pEffect = reinterpret_cast<BaseEffectClass*>(new VFXEffect(pDevice, L"resources/VFX.fx"));
		break;
	}

	
	HRESULT result;

	// create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertexData.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result)) return;

	// create index buffer
	m_NumIndeces = static_cast<uint32_t>(indexData.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndeces;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indexData.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	m_pEffect->SetWorldPosition(reinterpret_cast<const float*>(&m_Worldmatrix));

	if (FAILED(result)) return;

}

void Mesh::Render(ID3D11DeviceContext* pDeviceContex, const Matrix& viewProj)
{
	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContex->IASetInputLayout(m_pEffect->GetInputLayout());

	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContex->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	pDeviceContex->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContex);
		pDeviceContex->DrawIndexed(m_NumIndeces, 0, 0);
	}
	
	m_pEffect->SetMatrix( reinterpret_cast<const float*>(&viewProj));
}

Mesh::~Mesh()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
	}
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}
	if (m_pEffect)
	{
		delete m_pEffect;
	}
	
}
