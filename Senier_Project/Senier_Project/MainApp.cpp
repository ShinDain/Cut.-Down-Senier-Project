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
	std::unique_ptr<DWriteText> m_DWriteText;
	
	POINT mLastMousePos = { 0,0 };

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE previnstance, PSTR cmdLine, int showCmd)
{
	// �޸� ���� Ž��
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

	// ��ɸ�� �ʱ�ȭ
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	
	// ���� ���� �ʱ�ȭ
	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get()))
		return false;

	m_DWriteText = std::make_unique<DWriteText>();
	if (!m_DWriteText->Initialize(m_d2dDeviceContext.Get(), m_dWriteFactory.Get(), 25, D2D1::ColorF::Black))
		return false;
	m_DWriteText->AddTextUI(L"�ȳ�", 0, 0);
	m_DWriteText->AddTextUI(L"Hello", -100, 200);

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

	// ������Ʈ
	if (m_Scene)
	{
		//m_Scene->SetViewProjMatrix(viewProj4x4f);
		m_Scene->Update(gt);
	}
}

void MainApp::Render(const GameTimer& gt)
{
	FlushCommandQueue();
	
	auto cmdListalloc = m_DirectCmdListAlloc;

	// ��ɾ� �Ҵ���, ����Ʈ �ʱ�ȭ
	ThrowIfFailed(cmdListalloc->Reset());
	ThrowIfFailed(m_CommandList->Reset(cmdListalloc.Get(), NULL));

	// Viewport, ScissorRect ����
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Resource State ����
	D3D12_RESOURCE_BARRIER d3dResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// ���� Ÿ��, ����/���ٽ� ���� �ʱ�ȭ �� ����
	const D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView = CurrentBackBufferView();
	const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilBufferView = DepthStencilView();
	m_CommandList->ClearRenderTargetView(currentBackBufferView, Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilBufferView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, NULL);
	m_CommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilBufferView);

	// D3D12 Render �Լ� ȣ�� //////////////////////////////////////////
	if (m_Scene)
	{
		m_Scene->Render(gt, m_CommandList.Get());
		m_Scene->ImgObjRender(gt, m_CommandList.Get());
	}

	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// D3D12 Render�� ��� ������ �� 
	// D2D Render�� �����Ѵ�. D3D12 CommandList�� Execute�� �� �����ؾ� �Ѵ�.
	if (m_DWriteText)
	{
		m_DWriteText->Render(m_d3d11On12Device.Get(), m_d2dRenderTargets[m_CurrBackBuffer].Get(), m_d2dDeviceContext.Get(),
			m_d3d11DeviceContext.Get(), m_d3d11On12WrappedResoruces[m_CurrBackBuffer].Get());
	}

	// Resource State ����
	// D3D12_RESOURCE_BARRIER d3dResourceBarrier_Ren_Pre = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	// m_CommandList->ResourceBarrier(1, &d3dResourceBarrier_Ren_Pre);

	// Resource State�� D2D Render ���� �� Wrapped Resource�� Release�ϸ� �ڿ������� Present�� �ȴ�.
	
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





