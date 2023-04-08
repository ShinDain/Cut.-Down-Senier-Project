#pragma once

// 메모리 누수 탐지
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "D3DUtil.h"
#include "GameTimer.h"
#include "Global.h"

// d3d12 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")	
#pragma comment(lib, "dxgi.lib")

// d3d11on12 라이브러리
#pragma comment(lib, "D3D11.lib")

// d2d1 라이브러리
#pragma comment(lib, "D2D1.lib")

// dWrite 라이브러리
#pragma comment(lib, "Dwrite.lib")

class DirectXApp
{
protected:

	DirectXApp(HINSTANCE hInstance);
	DirectXApp(const DirectXApp& rhs) = delete;
	DirectXApp& operator=(const DirectXApp& rhs) = delete;
	virtual ~DirectXApp();

public:

	static DirectXApp* GetApp();

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

	virtual void ProcessInput() {}

protected:

	bool InitMainWindow();
	bool InitDirect3D();
	bool InitDirect3D11on12();
	bool InitDirect2DAndDirectWrite();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();
	void FlushCommandQueueAndReleaseBuffer();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	
	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:
	
	/// /////////////////////////////////////////////	

	static DirectXApp* m_App;

	HINSTANCE m_hAppInst = nullptr;
	HWND m_hMainWnd = nullptr;
	bool m_AppPaused = false;
	bool m_Minimized = false;
	bool m_Maximized = false;
	bool m_Resizing = false;
	BOOL m_FullscreenState = false;

	// 4x msaa 사용 여부, 기본은 false
	bool m_4xMsaaState = false;
	UINT m_4xMsaaQuality = 0;

	GameTimer m_Timer;

	/// /////////////////////////////////////////////	
	// D3D12

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;

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

	/// /////////////////////////////////////////////	
	// D3D11on12
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	Microsoft::WRL::ComPtr<ID3D11On12Device> m_d3d11On12Device;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_d3d11On12WrappedResoruces[SwapChainBufferCount];

	/// /////////////////////////////////////////////	
	// D2D components
	Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device> m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[SwapChainBufferCount];

	/// /////////////////////////////////////////////	
	// DWrite components
	Microsoft::WRL::ComPtr<IDWriteFactory> m_dWriteFactory;




	// 파생 클래스는 다음 파생 변수들을 초기화해줘야 한다.
	std::wstring m_MainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int m_ClientWidth = CLIENT_WIDTH;
	int m_ClientHeight = CLIENT_HEIGHT;

};

