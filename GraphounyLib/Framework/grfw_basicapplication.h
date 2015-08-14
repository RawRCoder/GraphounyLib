#pragma once
#include "..\gr_shared.h"
#include "grfw_keys.h"
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_3.h>
#include "grfw_config.h"
#include "grfwu_input.h"
#include "..\gr_rgba.h"

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

		KeyValueManager m_Config;
		rgba_s m_clrBackGround = RGBA_GREEN;
	protected:
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
		Microsoft::WRL::ComPtr<IDXGIFactory2> m_pDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pD3DBuffer[2];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCmdAlloc;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCmdQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCmdList;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescHeapRtv;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescHeapCbvSrvUAV;

		HANDLE m_hFenceEvent = nullptr;
		bool m_bWindowIsCreated = false;
		bool m_bDeviceIsCreated = false;
		time_t m_timeLastUpdate;
		LPRECT m_pWindowFullRect;
		LPRECT m_pWindowClientRect;
		POINT m_pntActualBufferSize;
		mouse_data_s m_MouseData;
		u32 m_iFrameIndex = 0;

		ID3D12Resource* m_pCurrentBuffer;
	};
} GRAPHOUNY_NAMESPACE_END

#define GetCFG(Key) g_pGlobals->CurrentApp()->m_Config.Get(L##Key)
#define GetTCFG(T, Key) g_pGlobals->CurrentApp()->m_Config.Get_##T(L##Key)
#define GetCFGD(Key, dv) g_pGlobals->CurrentApp()->m_Config.Get(L##Key, dv)
#define GetTCFGD(T, Key, dv) g_pGlobals->CurrentApp()->m_Config.Get_##T(L##Key, dv)
#define SetCFG(Key, V) g_pGlobals->CurrentApp()->m_Config.Set(L##Key, V)
#define SetTCFG(T, Key, V) g_pGlobals->CurrentApp()->m_Config.Set_##T(L##Key, V)