#include "Common/D3DApp.h"
#include "Common/MathHelper.h"


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
	virtual void Draw(const GameTimer& gt) override;
	
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	virtual void OnWinKeyboardInput(WPARAM wParam);
	void OnKeyboardInput(const GameTimer& gt);

private:
	
	//std::unique_ptr<Scene> mScenes;
	//std::unique_ptr<Player> mPlayer;

	
	POINT mLastMousePos = { 0,0 };

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE previnstance, PSTR cmdLine, int showCmd)
{
	// 메모리 누수 탐지
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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

}

MainApp::MainApp(HINSTANCE hInstance) : D3DApp(hInstance)
{
	
}

MainApp::~MainApp()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool MainApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// 명령목록 초기화
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// 각종 변수 초기화
	//
	//
	//

	// Execute
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void MainApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	// 업데이트

}

void MainApp::Draw(const GameTimer& gt)
{
	FlushCommandQueue();
	
	auto cmdListalloc = mDirectCmdListAlloc;

	// 명령어 할당자, 리스트 초기화
	ThrowIfFailed(cmdListalloc->Reset());
	ThrowIfFailed(mCommandList->Reset(cmdListalloc.Get(), NULL));

	// Viewport, ScissorRect 설정
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Resource State 변경
	D3D12_RESOURCE_BARRIER d3dResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// 렌더 타겟, 깊이/스텐실 버퍼 초기화 후 설정
	const D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView = CurrentBackBufferView();
	const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilBufferView = DepthStencilView();
	mCommandList->ClearRenderTargetView(currentBackBufferView, Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthStencilBufferView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, NULL);
	mCommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilBufferView);

	// Render 함수 호출
	//
	//
	//


	// Resource State 변경
	D3D12_RESOURCE_BARRIER d3dResourceBarrier_Ren_Pre = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &d3dResourceBarrier_Ren_Pre);

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

}


void MainApp::OnResize()
{
	D3DApp::OnResize();
}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{

	ReleaseCapture();
}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MainApp::OnWinKeyboardInput(WPARAM wParam)
{

}

void MainApp::OnKeyboardInput(const GameTimer& gt)
{

}





