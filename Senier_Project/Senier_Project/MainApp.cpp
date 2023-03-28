#include "Common/D3DApp.h"
#include "Common/MathHelper.h"
#include "Common/Scene.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	MainApp(const MainApp& rhs) = delete;
	MainApp& operator=(const MainApp & rhs) = delete;
	~MainApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Render(const GameTimer& gt) override;
	
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	virtual void OnWinKeyboardInput(WPARAM wParam);
	void OnKeyboardInput(const GameTimer& gt);

private:
	
	std::unique_ptr<Scene> mScene;
	//std::unique_ptr<Player> mPlayer;

	
	POINT mLastMousePos = { 0,0 };

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE previnstance, PSTR cmdLine, int showCmd)
{
	// 메모리 누수 탐지
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	if(S_OK != CoInitializeEx(NULL, COINIT_MULTITHREADED))
		return 0;

	try
	{
		MainApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;
		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR_Failed", MB_OK);
 		return 0;
	}

	CoUninitialize();

}

MainApp::MainApp(HINSTANCE hInstance) : D3DApp(hInstance)
{
	
}

MainApp::~MainApp()
{
	if (m_d3dDevice != nullptr)
		FlushCommandQueue();
}

bool MainApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// 명령목록 초기화
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	mScene = std::make_unique<Scene>();
	// 각종 변수 초기화
	if (!mScene->Initialize(m_d3dDevice.Get(), m_CommandList.Get()))
		return false;

	// Execute
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void MainApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	// 업데이트
	if (mScene)
	{
		//mScene->SetViewProjMatrix(viewProj4x4f);
		mScene->Update(gt);
	}
}

void MainApp::Render(const GameTimer& gt)
{
	FlushCommandQueue();
	
	auto cmdListalloc = m_DirectCmdListAlloc;

	// 명령어 할당자, 리스트 초기화
	ThrowIfFailed(cmdListalloc->Reset());
	ThrowIfFailed(m_CommandList->Reset(cmdListalloc.Get(), NULL));

	// Viewport, ScissorRect 설정
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Resource State 변경
	D3D12_RESOURCE_BARRIER d3dResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// 렌더 타겟, 깊이/스텐실 버퍼 초기화 후 설정
	const D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView = CurrentBackBufferView();
	const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilBufferView = DepthStencilView();
	m_CommandList->ClearRenderTargetView(currentBackBufferView, Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilBufferView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, NULL);
	m_CommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilBufferView);

	// Render 함수 호출 //////////////////////////////////////////
	if (mScene)
	{
		mScene->Render(gt, m_CommandList.Get());
	}

	// Render 함수 호출 //////////////////////////////////////////

	// Resource State 변경
	D3D12_RESOURCE_BARRIER d3dResourceBarrier_Ren_Pre = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &d3dResourceBarrier_Ren_Pre);

	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(m_SwapChain->Present(0, 0));
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;
}


void MainApp::OnResize()
{
	D3DApp::OnResize();
}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	mScene->OnMouseDown(btnState, x, y);

	SetCapture(m_hMainWnd);
}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	mScene->OnMouseUp(btnState, x, y);

	ReleaseCapture();
}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	mScene->OnMouseMove(btnState, x, y);
}

void MainApp::OnWinKeyboardInput(WPARAM wParam)
{
	if (mScene) mScene->OnWinKeyboardInput(wParam);
}

void MainApp::OnKeyboardInput(const GameTimer& gt)
{

}





