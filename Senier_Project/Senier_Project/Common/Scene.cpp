#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BuildRootSignature(pd3dDevice);

	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	//std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>();
	std::unique_ptr<Shader> defaultShader = std::make_unique<SkinnedMeshShader>();
	if (!defaultShader->Initialize(pd3dDevice, pd3dCommandList, m_RootSignature.Get(), NULL))
		return false;
	m_vpShaders.push_back(move(defaultShader));

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_RootSignature.Get(), NULL);

	// ModelData 로드 미완성
	//char strFileName[64] = "Model/Ethan.bin";
	char strFileName[64] = "Model/Angrybot.bin";
	//char strFileName[64] = "Model/Zebra.bin";
	//char strFileName[64] = "Model/Eagle.bin";
	//char strFileName[64] = "Model/unitychan.bin";
	//char strFileName[64] = "Model/Cube.bin";
	//char strFileName[64] = "Model/Humanoid.bin";

	std::shared_ptr<ModelDataInfo> tmpModel; 
	tmpModel = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, strFileName);

	m_vpObjs.emplace_back(std::make_shared<Object>(pd3dDevice, pd3dCommandList, tmpModel, 1));
	m_vpObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_vpObjs[0]->m_pAnimationController->SetTrackPosition(0, 0.2f);
	m_vpObjs[0]->SetPosition(10.0f, 0.0f, 0.0f);
	m_vpObjs[0]->SetScale(10.0f, 10.0f, 10.0f);
	m_vpObjs.emplace_back(std::make_shared<Object>(pd3dDevice, pd3dCommandList, tmpModel, 1));
	m_vpObjs[1]->SetPosition(0.0f, 0.0f, 0.0f);
	m_vpObjs[1]->SetScale(10.0f, 10.0f, 10.0f);


	m_pCamera = std::make_unique<Camera>();
	m_pCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));

	m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	return true;
}

void Scene::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	slotRootParameter[0].InitAsConstants(16, 0); // 월드 변환 행렬	// 오브젝트 상수 버퍼 (Material 상수 포함)
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[3].InitAsConstantBufferView(1);	// BoneOffsets 상수 버퍼 
	slotRootParameter[4].InitAsConstantBufferView(2);	// BoneTransforms 상수 버퍼 

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
}


void Scene::OnResize(float aspectRatio)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

}

void Scene::Update(const GameTimer& gt)
{
	for (int i = 0; i < m_vpShaders.size(); ++i)
	{
		m_vpShaders[i]->Update(gt);
	}

	XMFLOAT3 camPos3f = XMFLOAT3(0.0f, 0.0f, -100.f);
	XMVECTOR camPos = m_pCamera->GetPosition();
	XMStoreFloat3(&camPos3f, camPos);

	m_pCamera->LookAt(camPos3f, XMFLOAT3(0.0f, 0.0f, 0.0f), m_pCamera->GetUp3f());
	m_pCamera->UpdateViewMatrix();

	XMMATRIX viewProj = XMMatrixMultiply(m_pCamera->GetView(), m_pCamera->GetProj());

	PassConstant passConstant;
	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	m_pPassCB->CopyData(0, passConstant);

	for (int i = 0; i < m_vpObjs.size(); ++i)
	{
		m_vpObjs[i]->Update(gt);
	}
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

	/*for (int i = 0; i < m_vpShaders.size(); ++i)
	{
		m_vpShaders[i]->OnPrepareRender(pd3dCommandList);

		m_vpShaders[i]->Render(gt, pd3dCommandList);
	}*/

	for (int i = 0; i < m_vpObjs.size(); ++i)
	{
		m_vpObjs[i]->Animate(gt);
		if(!m_vpObjs[i]->m_pAnimationController)
			m_vpObjs[i]->UpdateTransform(NULL);
		m_vpObjs[i]->Render(gt, pd3dCommandList);
	}
}

void Scene::OnWinKeyboardInput(WPARAM wParam)
{
	for (int i = 0; i < m_vpShaders.size(); ++i)
		m_vpShaders[i]->OnWinKeyboardInput(wParam);

	float delta = 1.0f;

	if (wParam == 'W')
	{
		m_pCamera->Walk(delta);
	}

	if (wParam == 'S')
	{
		m_pCamera->Walk(-delta);
	}

	if (wParam == 'A')
	{
		m_pCamera->Strafe(-delta);
	}

	if (wParam == 'D')
	{
		m_pCamera->Strafe(delta);
	}
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(10.0f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(10.0f * static_cast<float>(y - m_LastMousePos.y));

		m_vpObjs[0]->SetRotate(m_vpObjs[0]->GetPitch() - dy, m_vpObjs[0]->GetYaw() - dx, 0);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> Scene::GetStaticSampler()
{
	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		0, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		1, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		anisotropicWrap, anisotropicClamp };
}