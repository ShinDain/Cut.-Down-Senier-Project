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

	// 명령목록 초기화
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	
	// 각종 변수 초기화
	g_Shaders.insert(std::make_pair<RenderLayer, std::shared_ptr<Shader>>(RenderLayer::Static, std::make_shared<Shader>()));
	g_Shaders.insert(std::make_pair<RenderLayer, std::shared_ptr<Shader>>(RenderLayer::Skinned, std::make_shared<SkinnedMeshShader>()));
	g_Shaders.insert(std::make_pair<RenderLayer, std::shared_ptr<Shader>>(RenderLayer::Image, std::make_shared<ImageObjectShader>()));
	g_Shaders.insert(std::make_pair<RenderLayer, std::shared_ptr<Shader>>(RenderLayer::Collider, std::make_shared<ColliderShader>()));

	for (auto iter = g_Shaders.begin(); iter != g_Shaders.end(); ++iter)
	{
		iter->second->Initialize(m_d3d12Device.Get(), m_CommandList.Get(), NULL);
	}

	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get()))
		return false;

	// 디버그 텍스트 객체 초기화
#if defined(_DEBUG) | defined(DEBUG)
	m_DebugText = std::make_unique<DWriteText>();
	if (!m_DebugText->Initialize(m_d2dDeviceContext.Get(), m_dWriteFactory.Get(), 25, D2D1::ColorF::Black, 
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING))
		return false;
	

	m_DebugText->AddTextUI(L"Total Time : 0", 10, 0);
	m_DebugText->AddTextUI(L"refCnt : 0", 10, 20);
	m_DebugText->AddTextUI(L"vecCnt : 0", 10, 40);

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
	ProcessInput();

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
	wchar_t totalTimeText[64] = {};
	// 시간 측정
	float posX = m_DebugText->GetTextUIPosX(nIdx);
	float posY = m_DebugText->GetTextUIPosY(nIdx);
	wcscpy_s(totalTimeText, L"Total Time : ");
	wcscat_s(totalTimeText, std::to_wstring(tTime).c_str());
	m_DebugText->UpdateTextUI(totalTimeText, posX, posY, nIdx);
	
	posX = m_DebugText->GetTextUIPosX(1);
	posY = m_DebugText->GetTextUIPosY(1);
	wcscpy_s(totalTimeText, L"refCnt : ");
	int refCnt = m_Scene->m_refCnt;
	wcscat_s(totalTimeText, std::to_wstring(refCnt).c_str());
	m_DebugText->UpdateTextUI(totalTimeText, posX, posY, 1);

	posX = m_DebugText->GetTextUIPosX(2);
	posY = m_DebugText->GetTextUIPosY(2);
	wcscpy_s(totalTimeText, L"vecCnt : ");
	int vecCnt = m_Scene->m_size;
	wcscat_s(totalTimeText, std::to_wstring(vecCnt).c_str());
	m_DebugText->UpdateTextUI(totalTimeText, posX, posY, 2);
#endif
}

void SenierProjectApp::Render(const GameTimer& gt)
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


void SenierProjectApp::OnResize()
{
	DirectXApp::OnResize();
}

void SenierProjectApp::ProcessInput()
{
	UCHAR keybuffer[256];
	GetKeyboardState(keybuffer);

	if(m_Scene) m_Scene->ProcessInput(keybuffer);

}




