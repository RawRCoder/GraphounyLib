#pragma once
#include <d3d12.h>
#include "..\..\gr_shared.h"
#include "wrl/client.h"

class PipeLine
{
public:
	PipeLine(ID3D12Device* device);
	void Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& descGr, D3D12_COMMAND_LIST_TYPE tp = D3D12_COMMAND_LIST_TYPE_DIRECT);
	void Reset();
	void Close();

	inline ID3D12GraphicsCommandList* GetGraphicsList() const { return m_pGrCommandList.Get(); }
	inline ID3D12PipelineState* GeState() const { return m_pState.Get(); }

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pAllocator;
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pGrCommandList;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pState;
	ID3D12Device* m_pDevice;
};
