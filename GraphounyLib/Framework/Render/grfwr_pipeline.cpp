#include "grfwr_pipeline.h"
#include "../grfwu_exception.h"

using namespace Graphouny::Framework;

PipeLine::PipeLine(ID3D12Device* device) : m_pDevice(device){}

void PipeLine::Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& descGr, D3D12_COMMAND_LIST_TYPE tp)
{
	ThrowOnFailed(
		m_pDevice->CreateGraphicsPipelineState(&descGr, IID_PPV_ARGS(&m_pState)), 
		L"Failed to create PSO");
	ThrowOnFailed(
		m_pDevice->CreateCommandList(0, tp, m_pAllocator.Get(), m_pState.Get(), IID_PPV_ARGS(&m_pGrCommandList)),
		L"Failed to create command list");
	Close();
}

void PipeLine::Reset()
{
	ThrowOnFailed(
		m_pAllocator->Reset(), 
		L"Failed to reset command allocator");
	ThrowOnFailed(
		m_pGrCommandList->Reset(m_pAllocator.Get(), m_pState.Get()),
		L"Failed to reset command list");
}

void PipeLine::Close()
{
	ThrowOnFailed(
		m_pGrCommandList->Close(),
		L"Failed to close command list");
}