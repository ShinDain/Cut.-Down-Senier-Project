#include "Common/DirectXApp.h"
#include "Common/MathHelper.h"
#include "Common/Scene.h"
#include "Common/DWriteText.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class MainApp : public DirectXApp
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
	
	std::unique_ptr<Scene> m_Scene;
	//std::unique_ptr<Player> m_Player;

#if defined(_DEBUG) | defined(DEBUG)
	std::unique_ptr<DWriteText> m_DebugText;
	
#endif

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

MainApp::MainApp(HINSTANCE hInstance) : DirectXApp(hInstance)
{
	
}

MainApp::~MainApp()
{
	if (m_d3d12Device != nullptr)
		FlushCommandQueue();
}

bool MainApp::Initialize()
{
	if (!DirectXApp::Initialize())
		return false;

	// 명령목록 초기화
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	
	// 각종 변수 초기화
	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get()))
		return false;

	// 디버그 텍스트 객체 초기화
#if defined(_DEBUG) | defined(DEBUG)
	m_DebugText = std::make_unique<DWriteText>();
	if (!m_DebugText->Initialize(m_d2dDeviceContext.Get(), m_dWriteFactory.Get(), 25, D2D1::ColorF::Black, 
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING))
		return false;
	
	// DWRITE_TEXT_ALIGNMENT_JUSTIFIED
	// DWRITE_TEXT_ALIGNMENT_CENTER
	// DWRITE_TEXT_ALIGNMENT_TRAILING
	// DWRITE_TEXT_ALIGNMENT_LEADING

	m_DebugText->AddTextUI(L"GameTimer : 0", 10, 0);
#endif

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
	if (m_Scene)
	{
		//m_Scene->SetViewProjMatrix(viewProj4x4f);
		m_Scene->Update(gt);
	}

	// 디버그를 위한 텍스트들 업데이트
#if defined(_DEBUG) | defined(DEBUG)

	// 전체 경과 시간 출력

	float tTime = m_Timer.TotalTime();
	int nIdx = 0;
	if (m_DebugText->GetTextUICount() > nIdx)
	{
		float posX = m_DebugText->GetTextUIPosX(nIdx);
		float posY = m_DebugText->GetTextUIPosY(nIdx);
		wchar_t totalTimeText[64] = {};
		wcscpy_s(totalTimeText, L"Total Time : ");
		wcscat_s(totalTimeText, std::to_wstring(tTime).c_str());
		m_DebugText->UpdateTextUI(&totalTimeText[0], posX, posY, nIdx);
	}	
#endif
	
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

	// D3D12 Render 함수 호출 //////////////////////////////////////////
	if (m_Scene)
	{
		m_Scene->Render(gt, m_CommandList.Get());
		m_Scene->ImgObjRender(gt, m_CommandList.Get());
	}

	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// D3D12 Render를 모두 종료한 후 
	// D2D Render를 진행한다. D3D12 CommandList를 Execute한 후 진행해야 한다.

#if defined(_DEBUG) | defined(DEBUG)
	if (m_DebugText)
	{
		m_DebugText->Render(m_d3d11On12Device.Get(), m_d2dRenderTargets[m_CurrBackBuffer].Get(), m_d2dDeviceContext.Get(),
			m_d3d11DeviceContext.Get(), m_d3d11On12WrappedResoruces[m_CurrBackBuffer].Get());
	}
#endif

	// Resource State 변경
	// D3D12_RESOURCE_BARRIER d3dResourceBarrier_Ren_Pre = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	// m_CommandList->ResourceBarrier(1, &d3dResourceBarrier_Ren_Pre);

	// Resource State는 D2D Render 종료 후 Wrapped Resource를 Release하며 자연스럽게 Present가 된다.
	
	ThrowIfFailed(m_SwapChain->Present(0, 0));
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;
}


void MainApp::OnResize()
{
	DirectXApp::OnResize();
}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	m_Scene->OnMouseDown(btnState, x, y);

	SetCapture(m_hMainWnd);
}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_Scene->OnMouseUp(btnState, x, y);

	ReleaseCapture();
}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	m_Scene->OnMouseMove(btnState, x, y);
}

void MainApp::OnWinKeyboardInput(WPARAM wParam)
{
	if (m_Scene) m_Scene->OnWinKeyboardInput(wParam);
}

void MainApp::OnKeyboardInput(const GameTimer& gt)
{

}





