#pragma once
#include "../../Utils/gru_array.h"
#include <wrl/client.h>
#include "../grfwu_exception.h"
#include "../../d3dx12.h"

template <class TVertex>
class VertexBuffer
{
public:
	VertexBuffer() {}

	inline void Push(TVertex vertex) { m_arrVerticies.Add(vertex); }
	inline void Push(u32 index) { m_arrIndicies.Add(index); }
	void Build(ID3D12Device* device)
	{
		const u32 TVERTEX_SZ = sizeof(TVertex);
		auto data_v = m_arrVerticies.ToFullArray();
		auto data_v_size = m_arrVerticies.Count() * TVERTEX_SZ;
		auto data_i = m_arrIndicies.ToFullArray();
		auto data_i_size = m_arrIndicies.Count() * 4;
		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		Graphouny::Framework::ThrowOnFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(data_v_size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pVBuffer)), L"Failed to create resource for vertex buffer");
		Graphouny::Framework::ThrowOnFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(data_i_size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pIBuffer)), L"Failed to create resource for index buffer");

		// Copy the triangle data to the vertex buffer.
		u8* pBuffer;
		Graphouny::Framework::ThrowOnFailed(
			m_pVBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pBuffer)),
			L"Failed to map vertex buffer");
		memcpy(pBuffer, data_v, sizeof(data_v));
		m_pVBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_VBufferView.BufferLocation = m_pVBuffer->GetGPUVirtualAddress();
		m_VBufferView.StrideInBytes = TVERTEX_SZ;
		m_VBufferView.SizeInBytes = data_v_size;


		// Copy the index data to the index buffer.
		Graphouny::Framework::ThrowOnFailed(
			m_pIBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pBuffer)),
			L"Failed to map index buffer");
		memcpy(pBuffer, data_i, sizeof(data_i));
		m_pIBuffer->Unmap(0, nullptr);

		// Initialize the index buffer view.
		m_IBufferView.BufferLocation = m_pIBuffer->GetGPUVirtualAddress();
		m_IBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IBufferView.SizeInBytes = data_i_size;
	}
	void Draw(ID3D12GraphicsCommandList* cmd)
	{
		cmd->IASetVertexBuffers(0, 1, &m_VBufferView);
		cmd->IASetIndexBuffer(&m_IBufferView);
		cmd->DrawIndexedInstanced(m_arrIndicies.Count(), 1, 0, 0, 0);
	}
	void Reset()
	{
		m_arrVerticies.Clear();
		m_arrIndicies.Clear();
		m_pVBuffer->Release();
		m_pIBuffer->Release();
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pIBuffer;
	Array<TVertex> m_arrVerticies;
	Array<u32> m_arrIndicies;
	D3D12_VERTEX_BUFFER_VIEW m_VBufferView;
	D3D12_INDEX_BUFFER_VIEW m_IBufferView;
};