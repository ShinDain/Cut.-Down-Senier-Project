#include "../Header/Scene.h"

ID3D12Device* Scene::m_pd3dDevice;
ID3D12GraphicsCommandList* Scene::m_pd3dCommandList;

std::vector<std::shared_ptr<Object>> Scene::m_vObjectLayer[(int)RenderLayer::Render_Count];
CollisionData Scene::m_CollisionData;
std::unique_ptr<CollisionResolver> Scene::m_pCollisionResolver;

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(MAX_CONTACT_CNT * 8);

	m_pd3dDevice = nullptr;
	m_pd3dCommandList = nullptr;
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<DWriteText> pDWriteText)
{
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;

	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);
	m_pShadowPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// 그림자 맵 생성
	m_ShadowMap = std::make_unique<DepthMap>(pd3dDevice, 2048, 2048);
	BuildDescriptorHeap(pd3dDevice, pd3dCommandList);

	// 캐릭터 테스트
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0,15, -20), XMFLOAT4(0,0,0,1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);

	// 무기
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);
	
	// 바닥
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);
	
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(200, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 200), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(200, 0, 200), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), GROUND_MODEL_NAME, 0);

	// 몬스터 테스트
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 100), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1.25, 1.25, 1.25),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(50, 0, 150), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(40, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1,1,1),ZOMBIE_MODEL_NAME, ZOMBIE_TRACK_CNT);

	// 월드 오브젝트 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(-40, 20, 40), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), WALL_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), SHELF_CRATE_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), SERVER_RACK_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(10, 5, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 5, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 15, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), VASE_MODEL_NAME, 0);
	
	// 아이템 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(100, 10, 20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), ITEM_MODEL_NAME, 0);
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 5, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), XMFLOAT3(1, 1, 1), PLAYER_PROJECTILE_MODEL_NAME, 0);

	// 맵 데이터 로드
	InitMapData(pd3dDevice, pd3dCommandList);
	// UI 초기화
	InitUI(pd3dDevice, pd3dCommandList, pDWriteText);
	// 시네마틱 초기화
	InitCinematic();

	// 카메라 초기화
	if (g_pPlayer)
	{
		m_pCamera = std::make_unique<Third_Person_Camera>(g_pPlayer);
		m_pCamera->Pitch(15);
	}

#if defined(_DEBUG)
	//m_pCamera = std::make_unique<Camera>();
	//m_pCamera->SetPosition(0, 30, -100);
	//m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

#endif

	return true;
}

bool Scene::InitMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadMapData(pd3dDevice, pd3dCommandList, "Map");

	return true;
}

bool Scene::InitUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<DWriteText> pDWriteText)
{
	// 각종 UI들 초기화  함수로 분리 예정
	m_pPlayerHPBar = std::make_unique<ImgObject>();
	m_pEnemyHPBar = std::make_unique<ImgObject>();
	m_pPlayerAim = std::make_unique<ImgObject>();

	m_pPlayerHPBar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"Textures/RedBack.dds", PLAYER_HP_BAR_WIDTH, PLAYER_HP_BAR_HEIGHT);
	m_pEnemyHPBar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"Textures/RedBack.dds", ENEMY_HP_BAR_WIDTH, ENEMY_HP_BAR_HEIGHT);
	m_pPlayerAim->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"Textures/Aim.dds", PLAYER_AIM_WIDTH, PLAYER_AIM_HEIGHT);
	m_pPlayerHPBar->ChangePosition(80, 35);
	m_pEnemyHPBar->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHPBar->GetBitmapWidth() / 2), 35);
	m_pPlayerAim->ChangePosition((CLIENT_WIDTH / 2) - (m_pPlayerAim->GetBitmapWidth() / 2), (CLIENT_HEIGHT / 2) - (m_pPlayerAim->GetBitmapHeight() / 2));
	m_pPlayerAim->SetVisible(false);
	// TextUI 초기화
	m_pTextUIs = pDWriteText;
	m_pTextUIs->AddTextUI(L"HP ", 35, -CLIENT_HEIGHT / 2 + 40);
	m_pTextUIs->AddTextUI(L"Score ", 35, -CLIENT_HEIGHT / 2 + 80);

	return true;
}

bool Scene::InitCinematic()
{
	m_pCinematicCamera = std::make_shared<Camera>();

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();
	pCinematic->SetCamera(m_pCinematicCamera);
	pCinematic->AddCameraKeyFrame(0.5f, XMFLOAT3(-20, -50, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0));
	pCinematic->AddCameraKeyFrame(1.0f, XMFLOAT3(20, 50, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0));
	pCinematic->AddCameraKeyFrame(1.5f, XMFLOAT3(20, 50, 20), XMFLOAT3(0, 90, 0), XMFLOAT3(0, 0, 0));
	pCinematic->AddCameraKeyFrame(1.7f, XMFLOAT3(20, 50, 20), XMFLOAT3(0, 90, 0), XMFLOAT3(0, 0, 0));

	m_vpCinematics.emplace_back(pCinematic);

	return true;
}

void Scene::BuildDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 2;			// ShdowMap, Dissolve Texture
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

	// Dissolve Texture
	hDescriptor.Offset(1, cbvSrvUavDescriptorSize);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	const wchar_t* pstrFilePath = L"Textures/Noise.dds";
	LoadTexture(pd3dDevice, pd3dCommandList, pstrFilePath);
	auto dissolveTexture = FindReplicatedTexture(pstrFilePath);
	srvDesc.Format = dissolveTexture->Resource->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = dissolveTexture->Resource->GetDesc().MipLevels;

	pd3dDevice->CreateShaderResourceView(dissolveTexture->Resource.Get(), &srvDesc, hDescriptor);
}

void Scene::OnResize(float aspectRatio, float newWidth, float newHeight)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

	//m_ShadowMap->OnResize(newWidth, newHeight);
}

void Scene::Update(float totalTime ,float elapsedTime)
{
	ClearObjectLayer();

#if defined(_DEBUG)
	XMFLOAT3 xmf3PlayerPosition = g_pPlayer->GetPosition();
	m_refCnt = xmf3PlayerPosition.x;
	//m_DebugValue = m_pCamera->GetPosition3f().x;

	m_tTime += elapsedTime;
#endif
	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (g_vpAllObjs[i]) g_vpAllObjs[i]->Update(elapsedTime);
	}
	//g_pPlayer->Update(elapsedTime);

	m_pCamera->Update(elapsedTime);
	m_vpCinematics[m_nCurCinematicNum]->Update(elapsedTime);
	if (m_vpCinematics[m_nCurCinematicNum]->GetCinematicEnd())
		m_bInCinematic = false;
	m_pCinematicCamera->Update(elapsedTime);

	// 패스버퍼 업데이트
	UpdateShadowPassCB(totalTime, elapsedTime);
	UpdatePassCB(totalTime, elapsedTime);

	// ImageObject 렌더를 위한  직교 투영행렬 업데이트
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

	// 충돌 검사
	GenerateContact();
	ProcessPhysics(elapsedTime);

	// UI 이미지 업데이트
	UpdateUI(elapsedTime);

	m_pPlayerHPBar->Update(elapsedTime);
	m_pEnemyHPBar->Update(elapsedTime);
	m_pPlayerAim->Update(elapsedTime);


#if defined(_DEBUG)
	xmf3PlayerPosition = g_pPlayer->GetPosition();
	m_DebugValue = xmf3PlayerPosition.x;
#endif
}

void Scene::UpdateUI(float elapsedTime)
{
	Player* pPlayer = (Player*)g_pPlayer.get();

	// 플레이어 HP 
	float playerHPRate = g_pPlayer->GetHP() / g_pPlayer->GetMaxHP();
	float playerHPBarWidth = PLAYER_HP_BAR_WIDTH * playerHPRate;
	m_pPlayerHPBar->ChangeSize(playerHPBarWidth, PLAYER_HP_BAR_HEIGHT);

	// 몬스터 HP
	Object* pTargetObject = pPlayer->GetPlayerTargetObject();
	if (pTargetObject && pTargetObject->GetObjectType() == Object_Monster)
	{
		m_pEnemyHPBar->SetVisible(true);

		float targetHPRate = pTargetObject->GetHP() / pTargetObject->GetMaxHP();
		float targetHPBarWidth = ENEMY_HP_BAR_WIDTH * targetHPRate;
		m_pEnemyHPBar->ChangeSize(targetHPBarWidth, ENEMY_HP_BAR_HEIGHT);
	}
	else
	{
		m_pEnemyHPBar->SetVisible(false);
	}

	// 점수 Text 
	wchar_t pstrScore[64] = L"Score : ";
	wcscat_s(pstrScore, std::to_wstring(pPlayer->GetScore()).c_str());

	m_pTextUIs->UpdateTextUI(pstrScore, m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Score), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Score), Text_UI_Idx_Score);

}

void Scene::UpdatePassCB(float totalTime, float elapsedTime)
{
	// 패스 버퍼 : 뷰 * 투영 변환 행렬 업데이트
	PassConstant passConstant;

	if (m_bInCinematic)
	{
		XMMATRIX view = m_pCinematicCamera->GetView();
		XMMATRIX viewProj = XMMatrixMultiply(view, m_pCinematicCamera->GetProj());
		passConstant.EyePosW = m_pCinematicCamera->GetPosition3f();
		XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	}
	else
	{
		XMMATRIX view = m_pCamera->GetView();
		XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());
		passConstant.EyePosW = m_pCamera->GetPosition3f();
		XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	}

	XMMATRIX shadowTransform = XMLoadFloat4x4(&m_xmf4x4ShadowTransform);
	XMStoreFloat4x4(&passConstant.ShadowTransform, XMMatrixTranspose(shadowTransform));

	passConstant.RenderTargetSize = XMFLOAT2((float)CLIENT_WIDTH, (float)CLIENT_HEIGHT);
	passConstant.InvRenderTargetSize = XMFLOAT2(1.0f / CLIENT_WIDTH, 1.0f / CLIENT_HEIGHT);
	passConstant.NearZ = 1.0f;
	passConstant.FarZ = 100.0f;
	passConstant.TotalTime = totalTime;
	passConstant.DeltaTime = elapsedTime;
	passConstant.AmbientLight = { 0.3f, 0.3f, 0.3f, 1.0f };
	passConstant.Lights[0].Direction = m_BaseLightDirections[0];
	passConstant.Lights[0].Strength = { 0.3f, 0.3f, 0.3f };
	passConstant.Lights[0].Position = { 0, 30.0f, 20 };

	passConstant.FadeInValue = m_FadeInValue;

	m_pPassCB->CopyData(0, passConstant);
}

void Scene::UpdateShadowPassCB(float totalTime, float elapsedTime)
{
	float sceneBoundRadius = 200;

	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&m_BaseLightDirections[0]);
	lightDir = XMVector3Normalize(lightDir);
	XMFLOAT3 xmf3PlayerPos = g_pPlayer->GetPosition();
	XMVECTOR targetPos = XMLoadFloat3(&xmf3PlayerPos);
	//XMVECTOR targetPos = XMVectorSet(0, 0, 0, 1);
	XMVECTOR lightPos = targetPos + -2.0f * sceneBoundRadius * lightDir;
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

	XMVECTOR tmpPosition = XMVectorSet(0, 0, 0, 1);
	tmpPosition = XMVector3TransformCoord(tmpPosition, shadowTransform);



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
	ChangeShader(ShaderType::Shader_Skinned, pd3dCommandList);
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

	//// 플레이어 렌더링
	//{
	//	g_pPlayer->Animate(elapsedTime);
	//	if (!g_pPlayer->m_pAnimationController)
	//		g_pPlayer->UpdateTransform(NULL);
	//	g_pPlayer->Render(elapsedTime, pd3dCommandList);
	//}

	ChangeShader(ShaderType::Shader_Static, pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_Static][i]->GetIsAlive())
			continue;
	
		m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_Static][i]->Render(elapsedTime, pd3dCommandList);
	}

	ChangeShader(ShaderType::Shader_TextureMesh, pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->Render(elapsedTime, pd3dCommandList);
	}

	ChangeShader(ShaderType::Shader_CuttedStatic, pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedStatic].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->Render(elapsedTime, pd3dCommandList);
	}

	ChangeShader(ShaderType::Shader_CuttedTextureMesh, pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedTexture].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->Render(elapsedTime, pd3dCommandList);
	}

	ChangeShader(ShaderType::Shader_CuttedSkinned, pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedSkinned].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->GetIsAlive())
			continue;
		// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->m_pAnimationController)
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Render(elapsedTime, pd3dCommandList);
	}

	// img 오브젝트
	{
		g_Shaders[ShaderType::Shader_Image]->ChangeShader(pd3dCommandList);
	
		//// Scene 그림자맵 테스트 시 주석 해제
		//ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
		//pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);
		//
		//D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		//pd3dCommandList->SetGraphicsRootDescriptorTable(1, texHandle);

		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);

		m_pPlayerHPBar->Render(elapsedTime, pd3dCommandList);
		m_pEnemyHPBar->Render(elapsedTime, pd3dCommandList);
		m_pPlayerAim->Render(elapsedTime, pd3dCommandList);
	}
}

void Scene::RenderSceneToShadowMap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//return;

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


	g_Shaders[ShaderType::Shader_Skinned]->ChangeShader(pd3dCommandList);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv = m_ShadowMap->Dsv();
	pd3dCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);
	pd3dCommandList->SetGraphicsRootConstantBufferView(m_nPassCBParameterIdx, m_pShadowPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
		{
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->GetIsAlive())
				continue;
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(0.0f);
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}

	// 셰이더 변경 여기
	g_Shaders[ShaderType::Shader_DepthMap]->ChangeShader(pd3dCommandList);
	//g_Shaders[ShaderType::Shader_Static]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Static][i]->GetShadowed())
		{
			m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Static][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}
	//g_Shaders[ShaderType::Shader_TextureMesh]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->GetShadowed())
		{
			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}

	// 잘린 면의 배제 없이 통째로 깊이가 그려지는 중
	//g_Shaders[ShaderType::Shader_CuttedStatic]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedStatic].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->DepthRender(0.0f, pd3dCommandList);
	}
	//g_Shaders[ShaderType::Shader_CuttedTextureMesh]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedTexture].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->DepthRender(0.0f, pd3dCommandList);
	}
	g_Shaders[ShaderType::Shader_CuttedSkinned]->ChangeShader(pd3dCommandList);
	//g_Shaders[ShaderType::Shader_Skinned]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedSkinned].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->GetIsAlive())
			continue;
		// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->m_pAnimationController)
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->DepthRender(0.0f, pd3dCommandList);
	}

	transition = CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	pd3dCommandList->ResourceBarrier(1, &transition);
}

void Scene::ChangeShader(ShaderType nShaderType, ID3D12GraphicsCommandList* pd3dCommandList)
{
	g_Shaders[nShaderType]->ChangeShader(pd3dCommandList);
	
	pd3dCommandList->SetGraphicsRootConstantBufferView(m_nPassCBParameterIdx, m_pPassCB->Resource()->GetGPUVirtualAddress());

	// Scene 그림자맵 
	ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_nDescTableParameterIdx, texHandle);
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if (m_pCamera)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		if (pKeybuffer[VK_RBUTTON] & 0xF0)
		{
			tmpCam->SetIsShoulderView(true);
			m_pPlayerAim->SetVisible(true);
		}
		else
		{
			tmpCam->SetIsShoulderView(false);
			m_pPlayerAim->SetVisible(false);
		}
		float dx, dy;
		dx = dy = 0;

		// 카메라 이동
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 25.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 25.0f;

		if (dx != 0 || dy != 0)
		{
			if (dx != 0 || dy != 0)
			{
				m_pCamera->Pitch(dy);
				m_pCamera->RotateY(dx);
			}
		}
	}

	if (g_pPlayer)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		Player* tmpPlayer = (Player*)(g_pPlayer.get());

		g_pPlayer->ProcessInput(pKeybuffer);

		if (tmpCam->GetIsShoulderView())
		{
			tmpPlayer->SetIsShoulderView(true);
			tmpPlayer->GetBody()->SetCharacterPitch(tmpCam->GetShoulderCameraPitch());
			tmpPlayer->SetRotate(XMFLOAT3(tmpCam->GetShoulderCameraPitch(), m_pCamera->GetYaw(), 0));
			tmpPlayer->SetCameraRotation(XMFLOAT3(tmpCam->GetShoulderCameraPitch(), m_pCamera->GetYaw(), 0));
		}
		else
		{
			tmpPlayer->SetIsShoulderView(false);
			tmpPlayer->GetBody()->SetCharacterPitch(0);
			tmpPlayer->SetCameraRotation(XMFLOAT3(0, m_pCamera->GetYaw(), 0));
		}
	}
	
#if defined(_DEBUG)

#endif
}

void Scene::KeyDownEvent(WPARAM wParam)
{
	switch (wParam)
	{
	case 'U':
		m_FadeInValue += 0.1f;
		if (m_FadeInValue > 1.0f)
			m_FadeInValue = 1.0f;
		break;
	case 'I':
		m_FadeInValue -= 0.1f;
		if(m_FadeInValue < 0.0f)
			m_FadeInValue = 0.0f;
		break;
	case 'P':
		PlayCinematic(0);
		break;
	}

	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if(g_pPlayer) g_pPlayer->KeyDownEvent(wParam);
}

void Scene::KeyUpEvent(WPARAM wParam)
{
	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if (g_pPlayer) g_pPlayer->KeyUpEvent(wParam);
}

void Scene::LeftButtonDownEvent()
{
	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if (g_pPlayer)
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->LeftButtonDownEvent();
	}
}

void Scene::RightButtonDownEvent()
{
	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if (g_pPlayer) 
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->RightButtonDownEvent();
	}
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
				if (pstrObjectName == g_DefaultObjectNames[i])
					pstrObjectName = g_DefaultObjectNames[i].c_str();
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
		objectData.bShadow = g_DefaultObjectData[pstrFileName].bShadowed;
	}
	
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;
	std::string strFileName;

	if (pstrFileName != nullptr)
	{
		strFileName = pstrFileName;

		if (g_LoadedModelData.find(pstrFileName) == g_LoadedModelData.end())
		{
			// 로드되지 않은 모델인 경우
			assert(false);
		}
		else	// 이미 로드한 모델인 경우
		{
			pModelData = g_LoadedModelData[pstrFileName];
		}
	}
	else if (pstrFileName == nullptr)
	{
		pModelData = nullptr;
		strFileName = "";
	}
	
	switch (objectData.objectType)
	{
	case Object_Player:
	{
		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		strcpy_s(pPlayer->m_pstrFileName, strFileName.c_str());
		//m_pPlayer = pPlayer;
		pObject = std::static_pointer_cast<Object>(pPlayer);

		g_vpAllObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pPlayer);
		g_pPlayer = pPlayer;
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;

	case Object_Monster:
	{
		std::shared_ptr<Monster> pMonster = std::make_shared<Monster>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pMonster);

		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;


	case Object_Weapon:
	{
		char tmp[64] = "mixamorig:RightHand";
		std::shared_ptr<Weapon> pWeapon = std::make_shared<Weapon>(pd3dDevice, pd3dCommandList, objectData, tmp, g_pPlayer, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pWeapon);
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->SetWeapon(pWeapon);

		g_vpAllObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;	

	case Object_Movable:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;

	case Object_Item:
	{
		std::shared_ptr<Item> pItem = std::make_shared<Item>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pItem);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;

	case Object_PlayerProjectile:
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pProjectile->SetProjectileProperty(ProjectileProperty::Projectile_Player);
		pObject = std::static_pointer_cast<Object>(pProjectile);
		
		g_vpAllObjs.emplace_back(pObject); 
		if (objectData.colliderType != Collider_None)
		{
			g_vpMovableObjs.emplace_back(pObject);
			g_vpWorldObjs.emplace_back(pObject);
		}
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;

	case Object_EnemyProjectile:
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pProjectile->SetProjectileProperty(ProjectileProperty::Projectile_Enemy);
		pObject = std::static_pointer_cast<Object>(pProjectile);

		g_vpAllObjs.emplace_back(pObject);
		if (objectData.colliderType != Collider_None)
		{
			g_vpMovableObjs.emplace_back(pObject);
			g_vpWorldObjs.emplace_back(pObject);
		}
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;

	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;
	}

	if (pObject && strFileName.c_str() != nullptr) strcpy_s(pObject->m_pstrFileName, strFileName.c_str());

	return pObject;
}

std::shared_ptr<Object> Scene::CreateCuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Object* pObject, float direction, XMFLOAT3 xmf3PlaneNormal, bool bIsCutted)
{
	float cuttingPower = rand() % 20 + 5; 
	XMFLOAT3 noiseDir;
	noiseDir.x = (float)(rand() % 30) / 100 * (1 - xmf3PlaneNormal.x);
	noiseDir.y = (float)(rand() % 30) / 100 * (1 - xmf3PlaneNormal.y);
	noiseDir.z = (float)(rand() % 30) / 100 * (1 - xmf3PlaneNormal.z);
	
	float directions[3] = {0,0,0};
	XMFLOAT3 xmf3PlaneNormals[3] = { XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(0,0,0) };
	UINT planeCnt = 0;

	if (bIsCutted)
	{
		CuttedObject* pCuttedObj = (CuttedObject*)(pObject);

		UINT lastPlaneCnt = pCuttedObj->GetPlaneCnt();
		if (lastPlaneCnt >= 3)
			return nullptr;
		for (int i = 0; i < lastPlaneCnt; ++i)
		{
			directions[i] = pCuttedObj->GetPlaneDirection(i);
			xmf3PlaneNormals[i] = pCuttedObj->GetPlaneNormal(i);
		}
		directions[lastPlaneCnt] = direction * -1;
		xmf3PlaneNormals[lastPlaneCnt] = xmf3PlaneNormal;
		planeCnt = lastPlaneCnt + 1;
		pCuttedObj->AddPlane(direction, xmf3PlaneNormal);


		XMFLOAT3 xmf3CuttingVel = xmf3PlaneNormal;
		xmf3CuttingVel.x *= cuttingPower * direction * -1 + noiseDir.x;
		xmf3CuttingVel.y *= cuttingPower * direction * -1 + noiseDir.y;
		xmf3CuttingVel.z *= cuttingPower * direction * -1 + noiseDir.z;
		pCuttedObj->GetBody()->AddVelocity(xmf3CuttingVel);
	}
	else
	{
		directions[0] = direction;
		xmf3PlaneNormals[0] = xmf3PlaneNormal;
		planeCnt += 1;
	}

	std::string pstrFileName;
	pstrFileName = pObject->m_pstrFileName;

	ObjectInitData objectData;

	objectData.xmf3Position = pObject->GetPosition();
	objectData.xmf3Rotation = pObject->GetRotation();
	objectData.xmf4Orientation = pObject->GetOrientation();
	objectData.xmf3Scale = pObject->GetScale();
	objectData.nMass = g_DefaultObjectData[pstrFileName].nMass;
	//objectData.nMass = g_DefaultObjectData[pstrFileName].nMass * 0.3f;
	objectData.objectType = g_DefaultObjectData[pstrFileName].objectType;
	objectData.colliderType = g_DefaultObjectData[pstrFileName].colliderType;
	objectData.xmf3Extents = g_DefaultObjectData[pstrFileName].xmf3Extents;
	objectData.xmf3MeshOffsetPosition = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetPosition;
	objectData.xmf3MeshOffsetRotation = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetRotation;

	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> tmpObject;

	pModelData = g_LoadedModelData[pstrFileName];

	switch (objectData.objectType)
	{
	//case Object_Player:
	//{
	//	std::shared_ptr<Object> tmpObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
	//
	//	g_vpCuttedSkinnedObjs.emplace_back(tmpObject);
	//	m_vObjectLayer[RenderLayer::Render_CuttedSkinned].emplace_back(tmpObject);
	//}
	//break;

	case Object_Monster:
	{
		int trackAnimationSet = pObject->m_pAnimationController->GetTrackAnimationSet(3);
	 	float trackPosition = pObject->m_pAnimationController->GetTrackPosition(3);

		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions,xmf3PlaneNormals, trackAnimationSet, trackPosition, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned].emplace_back(tmpObject);
		pCuttedObject->SetCuttedCBIdx(CUTTED_CB_SKINNED_IDX);
	}
	break;

	case Object_Movable:
	{
		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions, xmf3PlaneNormals, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_TextureMesh)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedTexture].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_TEXTURE_IDX);
		}
		else if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_Static)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedStatic].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_STATIC_IDX);
		}
	}
	break;

	default:
	{
		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions, xmf3PlaneNormals, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_TextureMesh)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedTexture].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_TEXTURE_IDX);
		}
		else if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_Static)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedStatic].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_STATIC_IDX);
		}
	}
	break;
	}

	XMFLOAT3 xmf3CuttingVel = xmf3PlaneNormal;
	xmf3CuttingVel.x *= cuttingPower * direction + noiseDir.x;
	xmf3CuttingVel.y *= cuttingPower * direction + noiseDir.y;
	xmf3CuttingVel.z *= cuttingPower * direction + noiseDir.z;
	tmpObject->GetBody()->AddVelocity(xmf3CuttingVel);

	return tmpObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if(g_vpAllObjs[i]->GetBody())
			g_vpAllObjs[i]->GetBody()->ClearContact();
	}
	if(g_pPlayer) g_pPlayer->GetBody()->ClearContact();

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

		std::shared_ptr<Character> pCharacter = std::static_pointer_cast<Character>(g_vpCharacters[k]);

		for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;

			if (characterBox == g_ppColliderBoxs[i]) continue;
			// 박스 검사 이전 가능 여부 선행 검사 
			// BoxAndBox 내부보다 먼저 하는 것이 더 빠름
			if (!characterBox->GetBoundingSphere().Intersects(g_ppColliderBoxs[i]->GetBoundingSphere()))
				continue;

			CollisionDetector::BoxAndBox(*characterBox, *g_ppColliderBoxs[i], m_CollisionData, pCharacter.get());
		}
	}

	m_CollisionData.friction = 0.9f;
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		XMVECTOR planeNormal = XMLoadFloat3(&g_ppColliderPlanes[i]->GetDirection());
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox) continue;
			//if (!colliderBox->GetBoundingSphere().Intersects(planeNormal))
			//	continue;

			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// 박스끼리 검사
	for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
	{
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;

			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box)
				continue;
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox || colliderBox == g_ppColliderBoxs[i]) continue;

			// 박스 검사 이전 가능 여부 선행 검사 
			// BoxAndBox 내부보다 먼저 하는 것이 더 빠름
			if(!colliderBox->GetBoundingSphere().Intersects(g_ppColliderBoxs[i]->GetBoundingSphere()))
				continue;
			
			CollisionDetector::BoxAndBox(*colliderBox, *g_ppColliderBoxs[i], m_CollisionData, nullptr);
		}
	}

	// 잘린 물체 검사
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		for (int k = 0; k < g_vpCuttedObjects.size(); ++k)
		{
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpCuttedObjects[k]->GetCollider());
			if (!colliderBox) continue;

			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}

}

void Scene::ProcessPhysics(float elapsedTime)
{
	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);
}

void Scene::PlayCinematic(UINT nCinematicNum)
{
	if (m_vpCinematics.size() < nCinematicNum)
		return;

	m_bInCinematic = true;
	m_nCurCinematicNum = nCinematicNum;
	m_vpCinematics[m_nCurCinematicNum]->Play();
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
	for (int i = 0; i < g_vpCuttedObjects.size(); ++i)
	{
		if (!g_vpCuttedObjects[i]->GetIsAlive())
		{
			g_vpCuttedObjects[i]->DestroyRunTime();
			g_vpCuttedObjects.erase(g_vpCuttedObjects.begin() + i);
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
