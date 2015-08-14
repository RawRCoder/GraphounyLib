#pragma once
#include "..\gr_shared.h"
#include "grfw_keys.h"
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <..\shared\dxgi1_4.h>
#include "grfw_config.h"
#include "grfwu_input.h"
#include "..\gr_rgba.h"
#include "../d3dx12.h"

class PipeLine;
GRAPHOUNY_NAMESPACE_FRAMEWORK {
	class Logger;
	class ContentManager;
	interface IReporter;

	// Basic class for any Graphouny application.
	class BasicApplication
	{
	public:
		const u32 BUFFER_COUNT = 2;
		const wchar_t* WND_CLASSNAME = L"GraphounyAppWindow";

		virtual std::wstring GetAppName() const { return L"<unnamed_graphouny_app>"; }
		virtual ~BasicApplication();

		void Run(HINSTANCE hInst, i32 cmdShow, std::wstring title);
		inline HWND GetWindowHandle() const { return m_hWindowHandle; }
		inline ID3D12Device* GetDevice() const { return m_pDevice; }
		inline Logger* GetDefaultLogger() const { return m_pLogger; }
		inline time_t GetTimeLastUpdate() const { return m_timeLastUpdate; }
		inline const mouse_data_s& GetMouseData() const { return m_MouseData; }
		inline i32 GetWidth() const { return m_pWindowClientRect->right - m_pWindowClientRect->left; }
		inline i32 GetHeight() const { return m_pWindowClientRect->bottom - m_pWindowClientRect->top; }
		inline f32 GetWidthF() const { return (f32)(m_pWindowClientRect->right - m_pWindowClientRect->left); }
		inline f32 GetHeightF() const { return (f32)(m_pWindowClientRect->bottom - m_pWindowClientRect->top); }

		void SetIcon(std::wstring path);
		void SetIcon(i32 IDI);

		KeyValueManager m_Config;
		rgba_s m_clrBackGround = RGBA_GREEN;
	protected:
		inline ContentManager* GetContent() const { return m_pContentManager; }

		virtual void Initialize() {  }
		virtual void LoadContent() {  }
		virtual void UnloadContent() {  }

		virtual void Update(f32 delta) {}
		virtual void Render(f32 delta) {}

		virtual void OnChar(wchar_t c) {}
		virtual void OnKeyPressed(KeyEventArgs args) {}
		virtual void OnKeyReleased(KeyEventArgs args) {}
		virtual void OnWhileKeyIsDown(KeyEventArgs args) {}
		virtual void OnWhileKeyIsUp(KeyEventArgs args) {}

		virtual void OnMouseKeyPressed(KeyEventArgs args) {}
		virtual void OnMouseKeyReleased(KeyEventArgs args) {}
		virtual void OnWhileMouseKeyIsDown(KeyEventArgs args) {}
		virtual void OnWhileMouseKeyIsUp(KeyEventArgs args) {}
		virtual void OnMouseWheel(f32 delta) {}
		virtual void OnMouseMove() {}

		virtual ContentManager* InitContentManager();
		virtual std::wstring GetLogFileName() const;
		virtual std::wstring GetConfigPath() const;

		ID3D12GraphicsCommandList* GetCommandList() const;
		inline ID3D12RootSignature* GetRootSignature() const { return m_pRootSignature.Get(); }
		inline const D3D12_VIEWPORT& GetViewPort() const { return m_viewport; }
		inline const D3D12_RECT& GetScissorRect() const { return m_scissorRect; }
		void Clear();

		void CreatePipeLine(const std::wstring id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, D3D12_COMMAND_LIST_TYPE tp = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void SetPipeLine(PipeLine* pl);
		bool SetPipeLine(const std::wstring id);
		PipeLine* GetPipeLine(std::wstring id);
		void SetResourceBarrier(ID3D12GraphicsCommandList* commandList,
			ID3D12Resource* res,
			D3D12_RESOURCE_STATES before,
			D3D12_RESOURCE_STATES after);

		Dictionary<std::wstring, PipeLine*> m_dictPipeLines{ Comparator<std::wstring> };
	private:
		LRESULT static CALLBACK WindowProcessor(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam);
		void Init_PreInit();
		void Init_LoadConfig(std::wstring filename);
		void Init_CreateWindow(HINSTANCE hInst, i32 cmdShow, std::wstring title);
		void Init_DirectX12();
		void UpdateMouseData(bool firstTime = false);
		void Render_Before();
		void Render_After();

		ContentManager* m_pContentManager;
		Logger* m_pLogger = nullptr;
		IReporter* m_pReporter = nullptr;
		std::wstring m_wsLaunchingDirectory;
		HWND m_hWindowHandle = nullptr;
		HINSTANCE m_hWindowsInstance = nullptr;	

		ID3D12Device* m_pDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pD3DBuffer[2];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCmdAlloc;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCmdQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescHeapRtv;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescHeapCbvSrvUAV;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
		PipeLine* m_pCurrentPipeLine = nullptr;
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		HANDLE m_hFenceEvent = nullptr;
		bool m_bWindowIsCreated = false;
		bool m_bDeviceIsCreated = false;
		time_t m_timeLastUpdate;
		LPRECT m_pWindowFullRect;
		LPRECT m_pWindowClientRect;
		POINT m_pntActualBufferSize;
		mouse_data_s m_MouseData;
		u32 m_iFrameIndex = 0;
		u32 m_iRTVDescSize;

		ID3D12Resource* m_pCurrentBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;
	};
} GRAPHOUNY_NAMESPACE_END

#define GetCFG(Key) g_pGlobals->CurrentApp()->m_Config.Get(L##Key)
#define GetTCFG(T, Key) g_pGlobals->CurrentApp()->m_Config.Get_##T(L##Key)
#define GetCFGD(Key, dv) g_pGlobals->CurrentApp()->m_Config.Get(L##Key, dv)
#define GetTCFGD(T, Key, dv) g_pGlobals->CurrentApp()->m_Config.Get_##T(L##Key, dv)
#define SetCFG(Key, V) g_pGlobals->CurrentApp()->m_Config.Set(L##Key, V)
#define SetTCFG(T, Key, V) g_pGlobals->CurrentApp()->m_Config.Set_##T(L##Key, V)