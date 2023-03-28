//***************************************************************************************
// D3DApp.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************
#pragma once

// 메모리 누수 탐지
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "D3DUtil.h"
#include "GameTimer.h"

// d3d12 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")	
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:

	static D3DApp* GetApp();

	HINSTANCE AppInst()const;
	HWND MainWnd()const;
	float AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParma, LPARAM lParam);

protected:

	virtual void CreateRtvAndDsvDescriptoHeaps();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Render(const GameTimer& gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
	virtual void OnWinKeyboardInput(WPARAM wParam) {}

protected:

	bool initMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	
	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:
	
	static D3DApp* m_App;

	HINSTANCE m_hAppInst = nullptr;
	HWND m_hMainWnd = nullptr;
	bool m_AppPaused = false;
	bool m_Minimized = false;
	bool m_Maximized = false;
	bool m_Resizing = false;
	bool m_FullscreenState = false;

	// 4x msaa 사용 여부, 기본은 false
	bool m_4xMsaaState = false;
	UINT m_4xMsaaQuality = 0;

	GameTimer m_Timer;

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_CurrentFence = 0;
	
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	static const int SwapChainBufferCount = 2;
	int m_CurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	// 파생 클래스는 다음 파생 변수들을 초기화해줘야 한다.
	std::wstring m_MainWndCaption = L"d3dd App";
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int m_ClientWidth = 1920;
	int m_ClientHeight = 1080;

};

