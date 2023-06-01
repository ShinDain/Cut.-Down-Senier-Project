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
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_Static, std::make_shared<Shader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_TextureMesh, std::make_shared<TextureMeshShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_Skinned, std::make_shared<SkinnedMeshShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_Image, std::make_shared<ImageObjectShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_WireFrame, std::make_shared<WireFrameShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_DepthMap, std::make_shared<DepthMapShader>()));

	for (auto iter = g_Shaders.begin(); iter != g_Shaders.end(); ++iter)
	{
		iter->second->Initialize(m_d3d12Device.Get(), m_CommandList.Get(), NULL);
	}

	// ������Ʈ �⺻ ���� ����
	CreateObjectDefaultData();

	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get()))
		return false;

	// ����� �ؽ�Ʈ ��ü �ʱ�ȭ
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

void SenierProjectApp::Update(float elapsedTime)
{
	ProcessInput();

	// ������Ʈ
	if (m_Scene)
	{
		//m_Scene->SetViewProjMatrix(viewProj4x4f);
		m_Scene->Update(m_Timer.TotalTime(), elapsedTime);
	}

	// ����׸� ���� �ؽ�Ʈ�� ������Ʈ
#if defined(_DEBUG) | defined(DEBUG)

	// ��ü ��� �ð� ���

	float tTime = m_Timer.TotalTime();
	int nIdx = 0;
	wchar_t totalTimeText[64] = {};
	// �ð� ����
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
	int vecCnt = g_vpAllObjs.size();
	wcscat_s(totalTimeText, std::to_wstring(vecCnt).c_str());
	m_DebugText->UpdateTextUI(totalTimeText, posX, posY, 2);
#endif
}

void SenierProjectApp::Render(float elapsedTime)
{
	FlushCommandQueue();
	
	auto cmdListalloc = m_DirectCmdListAlloc;

	// ��ɾ� �Ҵ���, ����Ʈ �ʱ�ȭ
	ThrowIfFailed(cmdListalloc->Reset());
	ThrowIfFailed(m_CommandList->Reset(cmdListalloc.Get(), NULL));

	// Shadow�� ����
	m_Scene->RenderSceneToShadowMap(m_CommandList.Get());

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
		m_Scene->Render(elapsedTime, m_CommandList.Get());
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

	if (m_Scene)
	{
	//	m_Scene->OnResize((float)m_ClientWidth / (float)m_ClientHeight, m_ClientWidth, m_ClientHeight);
		m_Scene->OnResize(1.5f, m_ClientWidth, m_ClientHeight);
	}
}

void SenierProjectApp::ProcessInput()
{
	UCHAR keybuffer[256];
	GetKeyboardState(keybuffer);

	if(m_Scene) m_Scene->ProcessInput(keybuffer);

}

void SenierProjectApp::KeyDownEvent(WPARAM wParam)
{
	if (m_Scene) m_Scene->KeyDownEvent(wParam);

}

void SenierProjectApp::LeftButtonDownEvent()
{
	if (m_Scene) m_Scene->LeftButtonDownEvent();
}

void SenierProjectApp::RightButtonDownEvent()
{
	if (m_Scene) m_Scene->RightButtonDownEvent();
}




