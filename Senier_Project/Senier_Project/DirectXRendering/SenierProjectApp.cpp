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
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_CuttedDepthMap, std::make_shared<CuttedDepthMapShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_CuttedStatic, std::make_shared<CuttedStaticMeshShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_CuttedTextureMesh, std::make_shared<CuttedTextureMeshShader>()));
	g_Shaders.insert(std::make_pair<ShaderType, std::shared_ptr<Shader>>(ShaderType::Shader_CuttedSkinned, std::make_shared<CuttedSkinnedMeshShader>()));

	for (auto iter = g_Shaders.begin(); iter != g_Shaders.end(); ++iter)
	{
		iter->second->Initialize(m_d3d12Device.Get(), m_CommandList.Get(), NULL);
	}

	// ������Ʈ �⺻ ���� ����
	CreateObjectDefaultData();
	 
	for (int i = 0; i < g_DefaultObjectNames.size(); ++i)
	{
		const char* pstrFileName = g_DefaultObjectNames[i].c_str();
		std::shared_ptr<ModelDataInfo> pModelData =
			Object::LoadModelDataFromFile(m_d3d12Device.Get(), m_CommandList.Get(), pstrFileName,
				g_DefaultObjectData[pstrFileName].pstrObjectPath, g_DefaultObjectData[pstrFileName].pstrTexPath);

		g_LoadedModelData.insert({ pstrFileName, pModelData });
	}

	// Scene �ʱ�ȭ
	m_pSceneTextUI = std::make_shared<DWriteText>();
	if (!m_pSceneTextUI->Initialize(m_d2dDeviceContext.Get(), m_dWriteFactory.Get(), 25, D2D1::ColorF::Black,
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING))
		return false;
	m_Scene = std::make_unique<Scene>();
	if (!m_Scene->Initialize(m_d3d12Device.Get(), m_CommandList.Get(), m_pSceneTextUI))
		return false;

	// device, commandlist ����
	g_pd3dDevice = m_d3d12Device.Get();
	g_pd3dCommandList = m_CommandList.Get();

#if defined(_DEBUG) | defined(DEBUG)

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

	FlushCommandQueue();

	g_tmpCaptionNum = 0;

	// ������Ʈ
	if (m_Scene)
	{
		//m_Scene->SetViewProjMatrix(viewProj4x4f);
		m_Scene->Update(m_Timer.TotalTime(), elapsedTime);
	}


#if defined(_DEBUG) | defined(DEBUG)
	
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
	if (m_pSceneTextUI)
	{
		m_pSceneTextUI->Render(m_d3d11On12Device.Get(), m_d2dRenderTargets[m_CurrBackBuffer].Get(), m_d2dDeviceContext.Get(),
			m_d3d11DeviceContext.Get(), m_d3d11On12WrappedResoruces[m_CurrBackBuffer].Get());
	}
	
#if defined(_DEBUG) | defined(DEBUG)

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

void SenierProjectApp::KeyUpEvent(WPARAM wParam)
{
	if (m_Scene) m_Scene->KeyUpEvent(wParam);
}

void SenierProjectApp::LeftButtonDownEvent()
{
	if (m_Scene) m_Scene->LeftButtonDownEvent();
}

void SenierProjectApp::RightButtonDownEvent()
{
	if (m_Scene) m_Scene->RightButtonDownEvent();
}




