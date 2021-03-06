#include <dxgi.h>
#include "grfw_shared.h"
#include "grfw_basicapplication.h"
#include <stdexcept>
#include "../d3dx12.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "grfwu_exception.h"
#include "Render/grfwr_pipeline.h"
using namespace std;

GRAPHOUNY_NAMESPACE_FRAMEWORK {

	HRESULT __enable_dx_debug()
	{
		ID3D12Debug* debug = nullptr;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		if (!debug || FAILED(hr))
			return hr;		
		debug->EnableDebugLayer();
		debug->Release();
		debug = nullptr;	
		return hr;
	}

	void BasicApplication::Init_DirectX12()
	{
		LogLn(L"[DBG]: Initializing DirectX 12...");
		LogLn(L"[DBG]: Client size: " + to_wstring(GetWidth()) + L"x" + to_wstring(GetHeight()));
		D3D_FEATURE_LEVEL levels[] = { 
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		std::wstring levelsNames[] = {
			L"D3D_FEATURE_LEVEL_12_1",
			L"D3D_FEATURE_LEVEL_12_0",
			L"D3D_FEATURE_LEVEL_11_1",
			L"D3D_FEATURE_LEVEL_11_0"
		}; 
#if _DEBUG
		ThrowOnFailed(
			CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf()))
			, L"Failed to create debug DXGI factory");
		ThrowOnFailed(
			__enable_dx_debug()
			, L"Failed to turn on DX debug layer");
#else
		ThrowOnFailed(
			CreateDXGIFactory2(0, IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf())), 
			L"Failed to create DXGI factory");
#endif
		auto hr = S_OK;
		auto maxFeatureLevels = sizeof(levels) / sizeof(D3D_FEATURE_LEVEL);
		for (u32 i = 0; i < maxFeatureLevels; ++i)
		{
			auto fl = levels[i];
			auto fln = levelsNames[i];
			hr = D3D12CreateDevice(nullptr, fl, IID_PPV_ARGS(&m_pDevice));
			if (SUCCEEDED(hr))
			{
				LogLn(L"[DBG]: Using highest avaliable feature level: " + fln);
				m_bDeviceIsCreated = true;
				break;
			}
		}
		ThrowOnFailed(hr, L"Failed to create DX12 Device: Min supported feature level is " + levelsNames[maxFeatureLevels-1]);

		ThrowOnFailed(
			m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCmdAlloc.ReleaseAndGetAddressOf())),
			L"Failed to create command allocator");

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowOnFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()))
			, L"Failed to create command queue");

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = BUFFER_COUNT;
		swapChainDesc.BufferDesc.Width = GetWidth();
		swapChainDesc.BufferDesc.Height = GetHeight();
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.OutputWindow = m_hWindowHandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE;

		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		ThrowOnFailed(m_pDxgiFactory->CreateSwapChain(
			m_pCmdQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
			&swapChainDesc,
			&swapChain
			), L"Failed to create spaw chain");

		ThrowOnFailed(swapChain.As(&m_pSwapChain), L"Failed to cast swap chain");

		ThrowOnFailed(
			m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()))
			, L"Failed to create fence");

		m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		for (u32 i = 0; i < BUFFER_COUNT; i++)
		{
			ThrowOnFailed(
				m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pD3DBuffer[i].ReleaseAndGetAddressOf()))
				, L"Failed to create buffer #" + to_wstring(i));
			m_pD3DBuffer[i]->SetName(L"SwapChain_Buffer");
		}

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = BUFFER_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowOnFailed(
			m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_pDescHeapRtv.ReleaseAndGetAddressOf()))
			, L"Failed to create RTV descriptor heap");
		m_iRTVDescSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Describe and create a shader resource view (SRV) heap for the texture.
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowOnFailed(
			m_pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pDescHeapCbvSrvUAV)), 
			L"Failed to create CBV/SRV/UAV descriptor heap");


		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescHeapRtv->GetCPUDescriptorHandleForHeapStart());
		for (auto i = 0u; i < BUFFER_COUNT; i++)
		{
			ThrowOnFailed(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pD3DBuffer[i])), L"Failed to get buffer");
			m_pDevice->CreateRenderTargetView(m_pD3DBuffer[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_iRTVDescSize);
		}

		m_viewport.Width = GetWidthF();
		m_viewport.Height = GetHeightF();
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.right = static_cast<i32>(GetWidth());
		m_scissorRect.bottom = static_cast<i32>(GetHeight());

		// Create an empty root signature.
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

			OnInitRootSignature(rootSignatureDesc);
		}

		m_iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void BasicApplication::OnInitRootSignature(CD3DX12_ROOT_SIGNATURE_DESC& rsdesc)
	{
		rsdesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		CreateRootSignature(rsdesc);
	}

	void BasicApplication::CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC& rsdesc)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		ThrowOnFailed(D3D12SerializeRootSignature(&rsdesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
			L"Failed to serialize root signature");
		ThrowOnFailed(m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)),
			L"Failed to create root signature");
	}

	ID3D12GraphicsCommandList* BasicApplication::GetCommandList() const
	{
		if (m_pCurrentPipeLine)
			return m_pCurrentPipeLine->GetGraphicsList();
		return nullptr;
	}

	void BasicApplication::Clear()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescHeapRtv->GetCPUDescriptorHandleForHeapStart(), m_iFrameIndex % BUFFER_COUNT, m_iRTVDescSize);
		GetCommandList()->ClearRenderTargetView(rtvHandle, rgba_s::to_floats(m_clrBackGround), 0, nullptr);
	}

	void BasicApplication::CreatePipeLine(const std::wstring id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, D3D12_COMMAND_LIST_TYPE tp)
	{
		auto ppl = new PipeLine(m_pDevice);
		ppl->m_pAllocator = m_pCmdAlloc.Get();
		ppl->Initialize(desc, tp);
		m_dictPipeLines.Set(id, ppl);
	}

	void BasicApplication::SetPipeLine(PipeLine* pl)
	{
		m_pCurrentPipeLine = pl;
	}

	PipeLine* BasicApplication::GetPipeLine(std::wstring id)
	{
		PipeLine* ppl;
		if (m_dictPipeLines.TryGet(id, &ppl))		
			return ppl;		
		return nullptr;
	}

	bool BasicApplication::SetPipeLine(const std::wstring id)
	{
		auto ppl = GetPipeLine(id);
		if (!ppl)
			return false;
		m_pCurrentPipeLine = ppl;
		return true;
	}

	void BasicApplication::SetResourceBarrier(ID3D12GraphicsCommandList* commandList,
		ID3D12Resource* res,
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after)
	{
		D3D12_RESOURCE_BARRIER desc = {};
		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource = res;
		desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore = before;
		desc.Transition.StateAfter = after;
		desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commandList->ResourceBarrier(1, &desc);
	}
	
	void BasicApplication::Render(f32 delta)
	{
		m_iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		auto ppl = m_pCurrentPipeLine;
		auto cmd = GetCommandList();

		ppl->Reset();
		m_pCurrentBuffer = m_pD3DBuffer[m_iFrameIndex % BUFFER_COUNT].Get();

		cmd->SetGraphicsRootSignature(m_pRootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { m_pDescHeapCbvSrvUAV.Get() };
		cmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		cmd->SetGraphicsRootDescriptorTable(0, m_pDescHeapCbvSrvUAV->GetGPUDescriptorHandleForHeapStart());
		cmd->RSSetViewports(1, &GetViewPort());
		cmd->RSSetScissorRects(1, &GetScissorRect());

		SetResourceBarrier(cmd, m_pCurrentBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescHeapRtv->GetCPUDescriptorHandleForHeapStart(), m_iFrameIndex % BUFFER_COUNT, m_iRTVDescSize);
		cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		OnRender(delta);
		SetResourceBarrier(cmd, m_pCurrentBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		ppl->Close();

		// Execute the command list.
		m_pCurRunningCommandList = cmd;
		m_pCmdQueue->ExecuteCommandLists(1, &m_pCurRunningCommandList);

		// Present
		ThrowOnFailed(m_pSwapChain->Present(1, 0), L"Failed to present a swap chain");

		WaitForGPU();
	}

	void BasicApplication::WaitForGPU()
	{
		const u64 fence = m_iFenceValue++;
		ThrowOnFailed(m_pCmdQueue->Signal(m_pFence.Get(), fence), L"Failed to set queue flushed event");
		if (m_pFence->GetCompletedValue() < fence)
		{
			ThrowOnFailed(m_pFence->SetEventOnCompletion(fence, m_hFenceEvent), L"Faile to wait");
			WaitForSingleObject(m_hFenceEvent, INFINITE);
		}
		m_pCurRunningCommandList = nullptr;
		m_iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void BasicApplication::StartCommands()
	{ 
		m_pCurrentPipeLine->Reset();
		m_bCommandsClosed = false;
	}

	void BasicApplication::ResetCommands()
	{
		ThrowOnFailed(GetCommandList()->Reset(m_pCmdAlloc.Get(), m_pCurrentPipeLine->GeState()), L"Failed to reset command list");
	}

	void BasicApplication::EndCommands()
	{
		m_pCurrentPipeLine->Close();
		m_bCommandsClosed = true;
	}

	void BasicApplication::ExecuteCommandsAndWait()
	{
		m_pCurRunningCommandList = GetCommandList();
		m_pCmdQueue->ExecuteCommandLists(1, &m_pCurRunningCommandList);
		WaitForGPU();
	}
} GRAPHOUNY_NAMESPACE_END