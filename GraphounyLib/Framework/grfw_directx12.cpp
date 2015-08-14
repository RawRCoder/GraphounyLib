#include "grfw_shared.h"
#include "grfw_basicapplication.h"
#include <stdexcept>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "grfwu_exception.h"
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
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowOnFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()))
			, L"Failed to create command queue");

		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Width = GetWidth();
		scDesc.Height = GetHeight();
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.SampleDesc.Count = 1;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = BUFFER_COUNT;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		ThrowOnFailed(
			m_pDxgiFactory->CreateSwapChainForHwnd(
				m_pCmdQueue.Get(), m_hWindowHandle, &scDesc, nullptr, nullptr, m_pSwapChain.ReleaseAndGetAddressOf())
			, L"Failed to create swap chain for window");

		ThrowOnFailed(
			m_pDevice->CreateCommandList(
				0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_pCmdAlloc.Get(), nullptr,
				IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()))
			, L"Failed to create command list");

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

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 10;
		desc.NodeMask = 0;
		ThrowOnFailed(
			m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pDescHeapRtv.ReleaseAndGetAddressOf()))
			, L"Failed to create descriptor heap");
		

		auto rtvStep = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (auto i = 0u; i < BUFFER_COUNT; i++)
		{
			auto d = m_pDescHeapRtv->GetCPUDescriptorHandleForHeapStart();
			d.ptr += i * rtvStep;
			m_pDevice->CreateRenderTargetView(m_pD3DBuffer[i].Get(), nullptr, d);
		}
	}

	void setResourceBarrier(ID3D12GraphicsCommandList* commandList,
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

	void BasicApplication::Render_Before()
	{
		// Get current RTV descriptor
		auto descHandleRtvStep = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE descHandleRtv = m_pDescHeapRtv->GetCPUDescriptorHandleForHeapStart();
		descHandleRtv.ptr += ((m_iFrameIndex) % BUFFER_COUNT) * descHandleRtvStep;
		// Get current swap chain
		m_pCurrentBuffer = m_pD3DBuffer[(m_iFrameIndex) % BUFFER_COUNT].Get();

		// Barrier Present -> RenderTarget
		setResourceBarrier(m_pCmdList.Get(), m_pCurrentBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Viewport
		D3D12_VIEWPORT viewport = {};
		viewport.Width = GetWidthF();
		viewport.Height = GetHeightF();
		m_pCmdList->RSSetViewports(1, &viewport);

		// Clear				
		m_pCmdList->ClearRenderTargetView(descHandleRtv, rgba_s::to_floats(m_clrBackGround), 0, nullptr);
	}

	void BasicApplication::Render_After()
	{
		// Barrier RenderTarget -> Present
		setResourceBarrier(m_pCmdList.Get(), m_pCurrentBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		// Exec
		ThrowOnFailed(m_pCmdList->Close(), L"Failed to execute command list");
		ID3D12CommandList* const cmdList = m_pCmdList.Get();
		m_pCmdQueue->ExecuteCommandLists(1, &cmdList);

		// Present
		ThrowOnFailed(m_pSwapChain->Present(1, 0), L"");

		// Set queue flushed event
		ThrowOnFailed(m_pFence->SetEventOnCompletion(m_iFrameIndex, m_hFenceEvent), L"");

		// Wait for queue flushed
		// This code would occur CPU stall!
		ThrowOnFailed(m_pCmdQueue->Signal(m_pFence.Get(), m_iFrameIndex), L"");
		DWORD wait = WaitForSingleObject(m_hFenceEvent, 10000);
		if (wait != WAIT_OBJECT_0)
			throw runtime_error("Failed WaitForSingleObject().");

		ThrowOnFailed(m_pCmdAlloc->Reset(), L"Failed to reset command allocator");
		ThrowOnFailed(m_pCmdList->Reset(m_pCmdAlloc.Get(), nullptr), L"Failed to reset command list");
		++m_iFrameIndex;
	}
} GRAPHOUNY_NAMESPACE_END