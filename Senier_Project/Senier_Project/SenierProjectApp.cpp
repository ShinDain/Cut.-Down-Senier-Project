#include "SenierProjectApp.h"

SenierProjectApp::SenierProjectApp(HINSTANCE hInstance) : DirectXApp(hInstance)
{
	
}

SenierProjectApp::~SenierProjectApp()
{
	if (m_d3d12Device != nullptr)
		FlushCommandQueue();
}

bool SenierProjectApp::Initialize()
{
	if (!DirectXApp::Initialize())
		return false;

	// ��ɸ�� �ʱ�ȭ
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	
	// ���� ���� �ʱ�ȭ
	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get()))
		return false;

	// ����� �ؽ�Ʈ ��ü �ʱ�ȭ
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

void SenierProjectApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	// ������Ʈ
	if (m_Scene)
	{
		//m_Scene->SetViewProjMatrix(viewProj4x4f);
		m_Scene->Update(gt);
	}

	// ����׸� ���� �ؽ�Ʈ�� ������Ʈ
#if defined(_DEBUG) | defined(DEBUG)

	// ��ü ��� �ð� ���

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

void SenierProjectApp::Render(const GameTimer& gt)
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

#if defined(_DEBUG) | defined(DEBUG)
	if (m_DebugText)
	{
		m_DebugText->Render(m_d3d11On12Device.Get(), m_d2dRenderTargets[m_CurrBackBuffer].Get(), m_d2dDeviceContext.Get(),
			m_d3d11DeviceContext.Get(), m_d3d11On12WrappedResoruces[m_CurrBackBuffer].Get());
	}
#endif

	// Resource State ����
	// D3D12_RESOURCE_BARRIER d3dResourceBarrier_Ren_Pre = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	// m_CommandList->ResourceBarrier(1, &d3dResourceBarrier_Ren_Pre);

	// Resource State�� D2D Render ���� �� Wrapped Resource�� Release�ϸ� �ڿ������� Present�� �ȴ�.
	
	ThrowIfFailed(m_SwapChain->Present(0, 0));
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;
}


void SenierProjectApp::OnResize()
{
	DirectXApp::OnResize();
}

void SenierProjectApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	m_Scene->OnMouseDown(btnState, x, y);

	SetCapture(m_hMainWnd);
}

void SenierProjectApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_Scene->OnMouseUp(btnState, x, y);

	ReleaseCapture();
}

void SenierProjectApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	m_Scene->OnMouseMove(btnState, x, y);
}

void SenierProjectApp::OnWinKeyboardInput(WPARAM wParam)
{
	if (m_Scene) m_Scene->OnWinKeyboardInput(wParam);
}

void SenierProjectApp::OnKeyboardInput(const GameTimer& gt)
{

}





