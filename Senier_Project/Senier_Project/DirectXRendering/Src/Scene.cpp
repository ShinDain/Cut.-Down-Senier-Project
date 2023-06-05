#include "../Header/Scene.h"

static float pEtime = 0;
CollisionData Scene::m_CollisionData;
std::unique_ptr<CollisionResolver> Scene::m_pCollisionResolver;

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(MAX_CONTACT_CNT * 8);

}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);
	m_pShadowPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// 그림자 맵 생성
	m_ShadowMap = std::make_unique<DepthMap>(pd3dDevice, 2048, 2048);
	BuildDescriptorHeap(pd3dDevice);

	// 캐릭터 테스트
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0,0,0), XMFLOAT4(0,0,0,1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);

	// 무기
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);

	// 바닥
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);

	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(200, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 200), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(200, 0, 200), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);


	// 몬스터 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 100), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(50, 0, 150), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(40, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);

	// 월드 오브젝트 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(-40, 0, 40), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), WALL_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), SHELF_CRATE_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), SERVER_RACK_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(10, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	
	// 아이템 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(100, 10, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), ITEM_MODEL_NAME, 0);

	// 맵 데이터 로드
	LoadMapData(pd3dDevice, pd3dCommandList, "Map");

	std::shared_ptr<ImgObject> imgobj = std::make_shared<ImgObject>();
	imgobj->Initialize(pd3dDevice, pd3dCommandList, 1900, 1024, L"Model/Textures/Carpet/Carpet_2_Diffuse.dds", 500, 500);
	m_pImage = imgobj;

	// 카메라 초기화
	if (m_pPlayer)
	{
		m_pCamera = std::make_unique<Third_Person_Camera>(m_pPlayer);
		m_pCamera->Pitch(15);
	}
#if defined(_DEBUG)
	//m_pCamera = std::make_unique<Camera>();
	//m_pCamera->SetPosition(0, 30, -100);
	//m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

#endif

	return true;
}

void Scene::BuildDescriptorHeap(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	m_ShadowMapHeapIndex = 0;

	auto srvCpuStart = m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	auto dsvCpuStart = m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	UINT cbvSrvUavDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT dsvDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	m_ShadowMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_ShadowMapHeapIndex, cbvSrvUavDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_ShadowMapHeapIndex, cbvSrvUavDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 0, dsvDescriptorSize));
}

void Scene::OnResize(float aspectRatio, float newWidth, float newHeight)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

	//m_ShadowMap->OnResize(newWidth, newHeight);
}

void Scene::Update(float totalTime ,float elapsedTime)
{
#if defined(_DEBUG)
	ClearObjectLayer();
	//m_refCnt = g_LoadedModelData[ZOMBIE_MODEL_NAME]->m_pRootObject.use_count();

	m_tTime += elapsedTime;

#endif

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (g_vpAllObjs[i]) g_vpAllObjs[i]->Update(elapsedTime);
	}

	m_pPlayer->Update(elapsedTime);

	// 충돌 검사
	GenerateContact();
	ProcessPhysics(elapsedTime);
	
	m_pCamera->Update(elapsedTime);

	// 패스버퍼 업데이트
	UpdatePassCB(totalTime, elapsedTime);
	UpdateShadowPassCB(totalTime, elapsedTime);

	// ImageObject 렌더를 위한  직교 투영행렬 업데이트
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();
}

void Scene::UpdatePassCB(float totalTime, float elapsedTime)
{
	// 패스 버퍼 : 뷰 * 투영 변환 행렬 업데이트
	PassConstant passConstant;

	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());

	XMMATRIX shadowTransform = XMLoadFloat4x4(&m_xmf4x4ShadowTransform);

	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&passConstant.ShadowTransform, XMMatrixTranspose(shadowTransform));

	passConstant.EyePosW = m_pCamera->GetPosition3f();
	passConstant.RenderTargetSize = XMFLOAT2((float)CLIENT_WIDTH, (float)CLIENT_HEIGHT);
	passConstant.InvRenderTargetSize = XMFLOAT2(1.0f / CLIENT_WIDTH, 1.0f / CLIENT_HEIGHT);
	passConstant.NearZ = 1.0f;
	passConstant.FarZ = 100.0f;
	passConstant.TotalTime = totalTime;
	passConstant.DeltaTime = elapsedTime;
	passConstant.AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	passConstant.Lights[0].Direction = m_BaseLightDirections[0];
	passConstant.Lights[0].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[0].Position = { 0, 30.0f, 20 };

	passConstant.Lights[1].Direction = m_BaseLightDirections[0];
	passConstant.Lights[1].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[1].Position = { 20, 20.0f, 120 };

	passConstant.Lights[2].Direction = m_BaseLightDirections[0];
	passConstant.Lights[2].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[2].Position = { -50, 20.0f, 240 };

	passConstant.Lights[3].Direction = m_BaseLightDirections[0];
	passConstant.Lights[3].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[3].Position = { 220, 20.0f, 0 };

	passConstant.Lights[4].Direction = m_BaseLightDirections[0];
	passConstant.Lights[4].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[4].Position = { 220, 20.0f, 120 };

	passConstant.Lights[5].Direction = m_BaseLightDirections[0];
	passConstant.Lights[5].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[5].Position = { 220, 20.0f, 240 };

	passConstant.Lights[6].Direction = m_BaseLightDirections[0];
	passConstant.Lights[6].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[6].Position = { 150, 20.0f, 0 };

	passConstant.Lights[7].Direction = m_BaseLightDirections[0];
	passConstant.Lights[7].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[7].Position = { 150, 20.0f, 120 };

	passConstant.Lights[8].Direction = m_BaseLightDirections[0];
	passConstant.Lights[8].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[8].Position = { 150, 20.0f, 240 };

	passConstant.Lights[9].Direction = m_BaseLightDirections[0];
	passConstant.Lights[9].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[9].Position = { 20, 20.0f, 240 };

	passConstant.Lights[10].Direction = m_BaseLightDirections[0];
	passConstant.Lights[10].Strength = { 0.5f, 0.5f, 0.5f };
	passConstant.Lights[10].Position = { -50, 20.0f, 150 };

	m_pPassCB->CopyData(0, passConstant);
}

void Scene::UpdateShadowPassCB(float totalTime, float elapsedTime)
{
	float sceneBoundRadius = 100;

	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&m_BaseLightDirections[0]);
	XMVECTOR lightPos = -2.0f * sceneBoundRadius * lightDir;
	XMVECTOR targetPos = XMVectorSet(0, 0, 0, 1);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - sceneBoundRadius;
	float b = sphereCenterLS.y - sceneBoundRadius;
	float lightNear = sphereCenterLS.z - sceneBoundRadius;
	float r = sphereCenterLS.x + sceneBoundRadius;
	float t = sphereCenterLS.y + sceneBoundRadius;
	float lightFar = sphereCenterLS.z + sceneBoundRadius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, lightNear, lightFar);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = lightView * lightProj * T;
	XMStoreFloat4x4(&m_xmf4x4ShadowTransform, shadowTransform);

	float shadowMapWidth = m_ShadowMap->Width();
	float shadowMapHeight = m_ShadowMap->Height();

	// 그림자 패스 버퍼 
	PassConstant passConstant;

	XMMATRIX view = lightView;
	XMMATRIX viewProj = XMMatrixMultiply(view, lightProj);

	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&passConstant.ShadowTransform, XMMatrixTranspose(shadowTransform));

	XMStoreFloat3(&passConstant.EyePosW, lightPos);
	passConstant.RenderTargetSize = XMFLOAT2((float)shadowMapWidth, (float)shadowMapHeight);
	passConstant.InvRenderTargetSize = XMFLOAT2(1.0f / shadowMapWidth, 1.0f / shadowMapHeight);
	passConstant.NearZ = lightNear;
	passConstant.FarZ = lightFar;
	passConstant.TotalTime = totalTime;
	passConstant.DeltaTime = elapsedTime;

	m_pShadowPassCB->CopyData(0, passConstant);
}

void Scene::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{

	g_Shaders[ShaderType::Shader_Static]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());
	//pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pShadowPassCB->Resource()->GetGPUVirtualAddress());
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_Static][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_Static][i]->Render(elapsedTime, pd3dCommandList);
	}

	g_Shaders[ShaderType::Shader_TextureMesh]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->Render(elapsedTime, pd3dCommandList);
	}

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
		{
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->GetIsAlive())
				continue;
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(elapsedTime);
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->Render(elapsedTime, pd3dCommandList);
		}
	}

	{
		m_pPlayer->Animate(elapsedTime);
		if (!m_pPlayer->m_pAnimationController)
			m_pPlayer->UpdateTransform(NULL);
		m_pPlayer->Render(elapsedTime, pd3dCommandList);
	}
	// img 오브젝트
	/*{
		g_Shaders[ShaderType::Shader_Image]->ChangeShader(pd3dCommandList);

		ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
		pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

		D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		pd3dCommandList->SetGraphicsRootDescriptorTable(1, texHandle);

		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);
		m_pImage->Render(elapsedTime, pd3dCommandList);
	}*/
}

void Scene::RenderSceneToShadowMap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	return;

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_VIEWPORT viewPort = m_ShadowMap->Viewport();
	D3D12_RECT scissorRect = m_ShadowMap->ScissorRect();
	pd3dCommandList->RSSetViewports(1, &viewPort);
	pd3dCommandList->RSSetScissorRects(1, &scissorRect);

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	pd3dCommandList->ResourceBarrier(1, &transition);

	pd3dCommandList->ClearDepthStencilView(m_ShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// 셰이더 변경 여기
	g_Shaders[ShaderType::Shader_DepthMap]->ChangeShader(pd3dCommandList);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv = m_ShadowMap->Dsv();
	pd3dCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pShadowPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_Static][i]->Render(0.0f, pd3dCommandList);
	}
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->Render(0.0f, pd3dCommandList);
	}

	transition = CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	pd3dCommandList->ResourceBarrier(1, &transition);
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	float dx, dy;
	dx = dy = 0;

	POINT ptCursorPos;

	if (pKeybuffer[VK_RBUTTON] & 0xF0)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 30.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 30.0f;
	}

	if (dx != 0 || dy != 0)
	{
		if (dx != 0 || dy != 0)
		{
			//m_vpAllObjs[0]->Rotate(0,dx, 0);

			m_pCamera->Pitch(dy);
			m_pCamera->RotateY(dx);
		}
	}
	
	// 임시
	if (m_pPlayer)
	{
		m_pPlayer->ProcessInput(pKeybuffer);
		m_pPlayer->SetCameraRotation(XMFLOAT3(0, m_pCamera->GetYaw(), 0));
	}
	
#if defined(_DEBUG)

#endif
}

void Scene::KeyDownEvent(WPARAM wParam)
{
	m_pPlayer->KeyDownEvent(wParam);
}

void Scene::LeftButtonDownEvent()
{
	if(m_pPlayer) m_pPlayer->LeftButtonDownEvent();
}

void Scene::RightButtonDownEvent()
{
	if (m_pPlayer) m_pPlayer->RightButtonDownEvent();
}

void Scene::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName)
{
	FILE* pInFile = NULL;

	char pstrFilePath[64] = { '\0' };
	strcpy_s(pstrFilePath, 64, "Map/");
	strcat_s(pstrFilePath, pstrFileName);
	strcat_s(pstrFilePath, ".bin");
	::fopen_s(&pInFile, pstrFilePath, "rb");
	::rewind(pInFile);

	char pstrToken[64] = { '\0' };

	const char* pstrObjectName = "\0";
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Rotation;
	XMFLOAT3 xmf3Scale;
	XMFLOAT4 xmf4Orientation;

	UINT nReads = 0;
	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (g_ppColliderBoxs.size() > 45)
		{
			float a = 100;
		}

		if(!strcmp(pstrToken, "<Hierarchy>:"))
		{
		} 
		else if (!strcmp(pstrToken, "<Frame>:"))
		{
			char tmpstr[64] = { '\0' };
			nReads = ReadStringFromFile(pInFile, tmpstr);

			if (tmpstr[nReads - 1] == ')')
			{
				for (int i = 1; i < 10; ++i)
				{
					if (tmpstr[nReads - i] == '(')
					{
						tmpstr[nReads - i] = '\0';
						tmpstr[nReads - i - 1] = '\0';
						break;
					}
					else
						tmpstr[nReads - i] = '\0';
				}
			}

			pstrObjectName = tmpstr;

			for (int i = 0; i < g_DefaultObjectNames.size(); ++i)
			{
				if (!strcmp(pstrObjectName, g_DefaultObjectNames[i]))
					pstrObjectName = g_DefaultObjectNames[i];
			}
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)fread(&xmf3Position, sizeof(float), 3, pInFile);
			xmf3Position.x *= 10;
			xmf3Position.y *= 10;
			xmf3Position.z *= 10;

			nReads = (UINT)fread(&xmf3Rotation, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&xmf4Orientation, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			if (!strcmp(pstrObjectName, ZOMBIE_MODEL_NAME))
				CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, xmf3Scale, pstrObjectName, ZOMBIE_TRACK_CNT);
			else
				CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, xmf3Scale, pstrObjectName, 0);
		}
		else if (!strcmp(pstrToken, "</Hierarchy>"))
		{
			break;
		}
		else
		{
			continue;
		}
	}

}


std::shared_ptr<Object> Scene::CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, const char* pstrFileName, int nAnimationTracks)
{
	ObjectInitData objectData;
	objectData.xmf3Position = xmf3Position;
	objectData.xmf3Rotation = xmf3Rotation;
	objectData.xmf4Orientation = xmf4Orientation;
	if (pstrFileName == nullptr)
	{
		objectData.nMass = 9999;
		objectData.objectType = Object_World;
		objectData.colliderType = Collider_Plane;
	}
	else
	{
		objectData.xmf3Scale = g_DefaultObjectData[pstrFileName].xmf3OffsetScale;
		objectData.xmf3Scale.x *= xmf3Scale.x;
		objectData.xmf3Scale.y *= xmf3Scale.y;
		objectData.xmf3Scale.z *= xmf3Scale.z;
		objectData.nMass = g_DefaultObjectData[pstrFileName].nMass;
		objectData.objectType = g_DefaultObjectData[pstrFileName].objectType;
		objectData.colliderType = g_DefaultObjectData[pstrFileName].colliderType;
		objectData.xmf3Extents = g_DefaultObjectData[pstrFileName].xmf3Extents;
		objectData.xmf3MeshOffsetPosition = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetPosition;
		objectData.xmf3MeshOffsetRotation = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetRotation;
	}
	
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;

	if (g_LoadedModelData.find(pstrFileName) == g_LoadedModelData.end() && pstrFileName != nullptr)
	{
		// 모델 로드
		pModelData = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, pstrFileName, g_DefaultObjectData[pstrFileName].pstrTexPath);

		g_LoadedModelData.insert({ pstrFileName, pModelData });
	}
	else if (pstrFileName == nullptr)
	{
		pModelData = nullptr;
	}
	else	// 이미 로드한 모델인 경우
	{
		pModelData = g_LoadedModelData[pstrFileName];
	}

	switch (objectData.objectType)
	{
	case Object_Player:
	{
		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		m_pPlayer = pPlayer;

		g_vpCharacters.emplace_back(pPlayer);
		g_pPlayer = pPlayer;
	}
		break;

	case Object_Monster:
	{
		std::shared_ptr<Monster> pMonster = std::make_shared<Monster>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pMonster);

		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	}
		break;


	case Object_Weapon:
	{
		char tmp[64] = "mixamorig:RightHand";
		std::shared_ptr<Weapon> pWeapon = std::make_shared<Weapon>(pd3dDevice, pd3dCommandList, objectData, tmp, m_pPlayer, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pWeapon);
		m_pPlayer->SetWeapon(pWeapon);

		g_vpAllObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	}
	break;	

	case Object_Movable:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	}
	break;

	case Object_Item:
	{
		std::shared_ptr<Item> pItem = std::make_shared<Item>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pItem);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	}
	break;

	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	}
		break;
	}

	return pObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if(g_vpAllObjs[i]->GetBody())
			g_vpAllObjs[i]->GetBody()->ClearContact();
	}
	m_pPlayer->GetBody()->ClearContact();

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// 캐릭터 검사
	for (int k = 0; k < g_vpCharacters.size(); ++k)
	{
		std::shared_ptr<ColliderBox> characterBox = std::static_pointer_cast<ColliderBox>(g_vpCharacters[k]->GetCollider());

		if (!characterBox) continue;

		for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*characterBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
		for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
		{
			if (characterBox == g_ppColliderBoxs[i]) continue;

			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndBox(*characterBox, *g_ppColliderBoxs[i], m_CollisionData);
		}

	}

	m_CollisionData.friction = 0.9f;
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox) continue;

			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// 박스끼리 검사
	for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
	{
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box)
				continue;
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox || colliderBox == g_ppColliderBoxs[i]) continue;

			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndBox(*colliderBox, *g_ppColliderBoxs[i], m_CollisionData);
		}
	}
}

void Scene::ProcessPhysics(float elapsedTime)
{
	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);
}

void Scene::ClearObjectLayer()
{
	for (int j = 0; j < g_ppColliderBoxs.size(); ++j)
	{
		if (g_ppColliderBoxs[j]->GetBody()->GetInvalid())
		{
			g_ppColliderBoxs.erase(g_ppColliderBoxs.begin() + j);
		}
	}
	for (int j = 0; j < g_ppColliderSpheres.size(); ++j)
	{
		if (g_ppColliderSpheres[j]->GetBody()->GetInvalid())
		{
			g_ppColliderSpheres.erase(g_ppColliderSpheres.begin() + j);
		}
	}
	for (int j = 0; j < g_ppColliderPlanes.size(); ++j)
	{
		if (g_ppColliderPlanes[j]->GetBody()->GetInvalid())
		{
			g_ppColliderPlanes.erase(g_ppColliderPlanes.begin() + j);
		}
	}

	// 전체 순회
	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (!g_vpAllObjs[i]->GetIsAlive())
		{
			g_vpAllObjs[i]->DestroyRunTime();
			g_vpAllObjs.erase(g_vpAllObjs.begin() + i);
		}
	}
	for (int i = 0; i < g_vpMovableObjs.size(); ++i)
	{
		if (!g_vpMovableObjs[i]->GetIsAlive())
		{
			g_vpMovableObjs[i]->DestroyRunTime();
			g_vpMovableObjs.erase(g_vpMovableObjs.begin() + i);
		}
	}
	for (int i = 0; i < g_vpCharacters.size(); ++i)
	{
		if (!g_vpCharacters[i]->GetIsAlive())
		{
			g_vpCharacters[i]->DestroyRunTime();
			g_vpCharacters.erase(g_vpCharacters.begin() + i);
		}
	}
	for (int i = 0; i < g_vpWorldObjs.size(); ++i)
	{
		if (!g_vpWorldObjs[i]->GetIsAlive())
		{
			g_vpWorldObjs[i]->DestroyRunTime();
			g_vpWorldObjs.erase(g_vpWorldObjs.begin() + i);
		}
	}

	// 레이어 순회
	for (int i = 0; i < RenderLayer::Render_Count; ++i)
	{
		for (int j = 0; j < m_vObjectLayer[i].size(); ++j)
		{
			if (!m_vObjectLayer[i][j]->GetIsAlive())
			{
				m_vObjectLayer[i][j]->DestroyRunTime();
				m_vObjectLayer[i].erase(m_vObjectLayer[i].begin() + j);
			}
		}
	}
}
