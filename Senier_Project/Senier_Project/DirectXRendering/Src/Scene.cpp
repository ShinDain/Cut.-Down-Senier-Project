#include "../Header/Scene.h"

UINT Scene::m_nStageNum = 0;
std::vector<std::shared_ptr<Object>> Scene::m_vObjectLayer[(int)RenderLayer::Render_Count];
CollisionData Scene::m_CollisionData;
std::unique_ptr<CollisionResolver> Scene::m_pCollisionResolver;

/// ////////////
std::shared_ptr<CSound> Scene::m_pMainBGM = nullptr;
std::vector<std::shared_ptr<CSound>> Scene::m_vpSounds;
/////////////

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(MAX_CONTACT_CNT * 8);
}

Scene::~Scene()
{
	m_pMainBGM.reset();
	for (int i = 0; i < m_vpSounds.size(); ++i)
		m_vpSounds[i].reset();
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	std::shared_ptr<DWriteText> pDWriteText, std::shared_ptr<DWriteText> pBigSizeText)
{
	// �н� ���� ����
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);
	m_pShadowPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// �׸��� �� ����
	m_ShadowMap = std::make_unique<DepthMap>(pd3dDevice, 2048, 2048);
	BuildDescriptorHeap(pd3dDevice, pd3dCommandList);

	// UI �ʱ�ȭ
	InitUI(pd3dDevice, pd3dCommandList, pDWriteText, pBigSizeText);

	// ���� ����
	GameStart();

	//// �÷��̾�
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 10, -20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);
	//
	//// ī�޶� �ʱ�ȭ
	//m_pCamera = nullptr;
	//m_pCamera = std::make_unique<Third_Person_Camera>(g_pPlayer);
	//m_pCamera->Pitch(15);
	//
	//// ����
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);
	//// �ٴ�
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);
	//
	//// ���� ������Ʈ �׽�Ʈ
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 20), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), ENTER_BOX_MODEL_NAME, 0);
	
	return true;
}

bool Scene::InitUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	std::shared_ptr<DWriteText> pDWriteText, std::shared_ptr<DWriteText> pBigSizeText)
{
	// ���� UI�� �ʱ�ȭ  �Լ��� �и� ����
	m_pPlayerHP_Bar = std::make_unique<ImgObject>();
	m_pPlayerHP_Frame = std::make_unique<ImgObject>();
	m_pPlayer_Aim = std::make_unique<ImgObject>();
	m_pEnemyHP_Bar = std::make_unique<ImgObject>();
	m_pEnemyHP_Frame = std::make_unique<ImgObject>();
	m_pEnemyName_Back = std::make_unique<ImgObject>();
	m_pHP_Back = std::make_unique<ImgObject>();
	m_pScore_Back = std::make_unique<ImgObject>();

	m_pPlayerHP_Bar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_line.dds", PLAYER_HP_BAR_WIDTH, PLAYER_HP_BAR_HEIGHT);
	m_pPlayerHP_Frame->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_frame.dds", PLAYER_HP_BAR_WIDTH, PLAYER_HP_BAR_HEIGHT);
	m_pPlayer_Aim->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Aim.dds", PLAYER_AIM_WIDTH, PLAYER_AIM_HEIGHT);
	m_pEnemyHP_Bar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_line.dds", ENEMY_HP_BAR_WIDTH, ENEMY_HP_BAR_HEIGHT);
	m_pEnemyHP_Frame->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_frame.dds", ENEMY_HP_BAR_WIDTH, ENEMY_HP_BAR_HEIGHT);
	m_pEnemyName_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Name_Back.dds", ENEMY_NAME_WIDTH, ENEMY_NAME_HEIGHT);
	m_pHP_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Score_Back.dds", HP_BACK_WIDTH, HP_BACK_HEIGHT);
	m_pScore_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Score_Back.dds", SCORE_BACK_WIDTH, SCORE_BACK_HEIGHT);

	m_pPlayerHP_Bar->ChangePosition(80, 30);
	m_pPlayerHP_Frame->ChangePosition(80, 30);
	m_pPlayer_Aim->ChangePosition((CLIENT_WIDTH / 2) - (m_pPlayer_Aim->GetBitmapWidth() / 2), (CLIENT_HEIGHT / 2) - (m_pPlayer_Aim->GetBitmapHeight() / 2));
	m_pEnemyHP_Bar->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 65);
	m_pEnemyHP_Frame->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 65);
	m_pEnemyName_Back->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 25);
	m_pHP_Back->ChangePosition(-3, 21);
	m_pScore_Back->ChangePosition(0, 63);

	// TextUI �ʱ�ȭ
	m_pTextUIs = pDWriteText;
	m_pTextUIs->AddTextUI(L"HP ", -CLIENT_WIDTH / 2 + 35, -CLIENT_HEIGHT / 2 + 40);
	m_pTextUIs->AddTextUI(L"Score ", -CLIENT_WIDTH / 2 + 65, -CLIENT_HEIGHT / 2 + 81);
	m_pTextUIs->AddTextUI(L"Monster Name", 0, -CLIENT_HEIGHT / 2 + 47);
	m_pTextUIs->AddTextUI(L"Loading...", CLIENT_WIDTH / 2 - 100, CLIENT_HEIGHT / 2 - 40);
	m_pTextUIs->AddTextUI(L"Press Any Key To Start", 0, CLIENT_HEIGHT / 3 - 80);
	m_pTextUIs->AddTextUI(L"Press 'ESC' To Exit", 0, CLIENT_HEIGHT / 3 - 40);
	m_pTextUIs->AddTextUI(L"Thank you for playing.", 0, 70);

	// ū ũ���� �ؽ�Ʈ
	m_pBigSizeTextUI = pBigSizeText;
	m_pBigSizeTextUI->AddTextUI(L"", 450, -250);
	m_pBigSizeTextUI->AddTextUI(L"", 0, -30);
	m_pBigSizeTextUI->AddTextUI(L"", 0, 0);
	m_pBigSizeTextUI->AddTextUI(L"", 0, 0);

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

	m_pCinematicCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.f);

}

void Scene::Update(float totalTime ,float elapsedTime)
{
	if(m_bInCinematic)
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);

	// m_bNextStage ������ �� �Ͻ�,
	ChangeStage();
	// m_bGameOver ������ �� �Ͻ�,
	GameOver();
	// m_bGameEnd ������ �� �Ͻ�,
	GameEnd();

	if (g_pEnterObject)
	{
		if (g_pEnterObject->GetIntersect())
		{
			m_bNextStage = true;
			g_pEnterObject->DestroyRunTime();
			g_pEnterObject = nullptr;
		}
	}
		
	if (m_bPaused)
	{
		// UI �̹��� ������Ʈ
		UpdateUI(elapsedTime);
		// ī�޶�
		UpdateSceneCamera(0.0f);
		// �н����� ������Ʈ
		UpdatePassCB(totalTime, elapsedTime);
		return;
	}

	// ȭ�� ��ȯ ȿ��
	UpdateFadeInOut(elapsedTime);
	// ������Ʈ
	UpdateObject(elapsedTime);
	// ī�޶�
	UpdateSceneCamera(elapsedTime);
	// �÷��̾�
	UpdatePlayerData(elapsedTime);
	// �н����� ������Ʈ
	UpdateShadowPassCB(totalTime, elapsedTime);
	UpdatePassCB(totalTime, elapsedTime);
	// ImageObject ������ ����  ���� ������� ������Ʈ
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

	// �浹 �˻�
	GenerateContact();
	ProcessPhysics(elapsedTime);
	// UI �̹��� ������Ʈ
	UpdateUI(elapsedTime);
	// Sound ������Ʈ
	UpdateSound();
	// �̺�Ʈ ��ü ������Ʈ
	UpdateEvent(elapsedTime);
}

void Scene::UpdateObject(float elapsedTime)
{
	// ������Ʈ ����
	ClearObjectLayer();

	// ī�޶� �������Ұ� ������ �����Ͽ� ������ ���� ������Ʈ
	BoundingFrustum* camFus;
	
	if(m_bInCinematic)
		camFus = m_pCinematicCamera->m_pCameraFrustum.get();
	else
		camFus = m_pCamera->m_pCameraFrustum.get();

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (!g_vpAllObjs[i]->GetIsAlive()) continue;

		if (g_vpAllObjs[i]->m_pAnimationController)
		{
			g_vpAllObjs[i]->SetVisible(false);
			g_vpAllObjs[i]->Animate(elapsedTime);
		}

		if (g_vpAllObjs[i]->GetColliderType() == Collider_Box)
		{
			BoundingSphere* pObjectBS = g_vpAllObjs[i]->GetCollider()->GetBoundingSphere().get();

			if (camFus->Intersects(*pObjectBS))
				g_vpAllObjs[i]->SetVisible(true);
			else
				g_vpAllObjs[i]->SetVisible(false);
		}

		if (g_vpAllObjs[i]) g_vpAllObjs[i]->Update(elapsedTime);
	}
}

void Scene::UpdateUI(float elapsedTime)
{
	Player* pPlayer = (Player*)g_pPlayer.get();

	// �÷��̾� HP 
	float playerHPRate = g_pPlayer->GetHP() / g_pPlayer->GetMaxHP();
	float playerHPBarWidth = PLAYER_HP_BAR_WIDTH * playerHPRate;
	m_pPlayerHP_Bar->ChangeSize(playerHPBarWidth, PLAYER_HP_BAR_HEIGHT);

	// ���� HP
	Object* pTargetObject = pPlayer->GetPlayerTargetObject();
	if (pTargetObject && pTargetObject->GetObjectType() == Object_Monster)
	{
		m_pEnemyHP_Bar->SetVisible(true);
		m_pEnemyHP_Frame->SetVisible(true);
		m_pEnemyName_Back->SetVisible(true);

		float targetHPRate = pTargetObject->GetHP() / pTargetObject->GetMaxHP();
		float targetHPBarWidth = ENEMY_HP_BAR_WIDTH * targetHPRate;
		m_pEnemyHP_Bar->ChangeSize(targetHPBarWidth, ENEMY_HP_BAR_HEIGHT);
	}
	else
	{
		m_pEnemyHP_Bar->SetVisible(false);
		m_pEnemyHP_Frame->SetVisible(false);
		m_pEnemyName_Back->SetVisible(false);
	}

	// Text UI ������Ʈ
	if (m_FadeInValue >= 1.0f && !m_bPaused)
	{
		if (!m_bInCinematic)
		{
			m_pPlayerHP_Bar->SetVisible(true);
			m_pPlayerHP_Frame->SetVisible(true);
			m_pPlayer_Aim->SetVisible(true);

			m_pHP_Back->SetVisible(true);
			m_pScore_Back->SetVisible(true);

			m_pTextUIs->UpdateTextUI(L"HP ", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_HP), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_HP), Text_UI_Idx_HP);

			// ���� Text ����
			wchar_t pstrScore[64] = L"Score : ";
			wcscat_s(pstrScore, std::to_wstring(pPlayer->GetScore()).c_str());
			int length = wcslen(pstrScore) - 9;
			m_pTextUIs->UpdateTextUI(pstrScore, -CLIENT_WIDTH / 2 + 65 + (length * 7), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Score), Text_UI_Idx_Score);

			// ���� �̸� ���
			wchar_t pstrName[64] = L"";
			if (pTargetObject && pTargetObject->GetObjectType() == Object_Monster)
			{
				wcscpy_s(pstrName, pTargetObject->GetOutName());
			}
			m_pTextUIs->UpdateTextUI(pstrName, m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Monster_Name), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Monster_Name), Text_UI_Idx_Monster_Name);
		}
		
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Loading), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Loading), Text_UI_Idx_Loading);
	}
	else
	{
		m_pPlayerHP_Bar->SetVisible(false);
		m_pPlayerHP_Frame->SetVisible(false);
		m_pPlayer_Aim->SetVisible(false);
		m_pEnemyHP_Bar->SetVisible(false);
		m_pEnemyHP_Frame->SetVisible(false);
		m_pEnemyName_Back->SetVisible(false);
		m_pHP_Back->SetVisible(false);
		m_pScore_Back->SetVisible(false);

		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_HP), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_HP), Text_UI_Idx_HP);
		m_pTextUIs->UpdateTextUI(L"", -CLIENT_WIDTH / 2 + 65, m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Score), Text_UI_Idx_Score);
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Monster_Name), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Monster_Name), Text_UI_Idx_Monster_Name);
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
	}

	if (m_bPressAnyKey && m_FadeState == 2)
		m_pTextUIs->UpdateTextUI(L"Press Any Key To Start", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
	else
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);

	if (m_bThanks && m_FadeState == 2)
		m_pTextUIs->UpdateTextUI(L"Thank you for playing.", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Thanks), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Thanks), Text_UI_Idx_Thanks);
	else
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Thanks), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Thanks), Text_UI_Idx_Thanks);

	if(m_bTitle && m_FadeState == 2)
		m_pBigSizeTextUI->UpdateTextUI(L"CUT. DOWN.",
			m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
			m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title), 
			Big_Text_UI_Idx_Title);
	else
		m_pBigSizeTextUI->UpdateTextUI(L"",
			m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
			m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title),
			Big_Text_UI_Idx_Title);

	if (m_bPaused)
	{
		for (int i = 0; i < Text_UI_Idx_Count; ++i)
		{
			m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(i), m_pTextUIs->GetTextUIPosY(i), i);
		}
		for (int i = 0; i < Big_Text_UI_Idx_Count; ++i)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"", m_pBigSizeTextUI->GetTextUIPosX(i), m_pBigSizeTextUI->GetTextUIPosY(i), i);
		}

		m_pTextUIs->UpdateTextUI(L"Press Any Key To Start", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
		m_pTextUIs->UpdateTextUI(L"Press 'ESC' To Exit", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_ESC), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_ESC), Text_UI_Idx_ESC);
		m_pBigSizeTextUI->UpdateTextUI(L"Paused", m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Paused), m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Paused), Big_Text_UI_Idx_Paused);
	}
	else
	{
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_ESC), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_ESC), Text_UI_Idx_ESC);
		m_pBigSizeTextUI->UpdateTextUI(L"", m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Paused), m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Paused), Big_Text_UI_Idx_Paused);
	}


	// Scene �̹��� UI ������Ʈ
	m_pPlayerHP_Bar->Update(elapsedTime);
	m_pPlayerHP_Frame->Update(elapsedTime);
	m_pEnemyHP_Bar->Update(elapsedTime);
	m_pEnemyHP_Frame->Update(elapsedTime);
	m_pPlayer_Aim->Update(elapsedTime);
	m_pEnemyName_Back->Update(elapsedTime);
	m_pHP_Back->Update(elapsedTime);
	m_pScore_Back->Update(elapsedTime);
}

void Scene::UpdateSound()
{
	if (m_pMainBGM)
	{
		m_pMainBGM->Update();
	}
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		if(m_vpSounds[i])
			m_vpSounds[i]->Update();
	}
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		// �ܹ߼� ȿ���� �����
		if (!m_vpSounds[i]->GetIsPlaying())
		{
			m_vpSounds[i].reset();
			m_vpSounds.erase(m_vpSounds.begin() + i);
		}
	}
}

void Scene::UpdateEvent(float elapsedTime)
{
	if (m_pEvnetObjects.size() == 0)
	{
		if (g_vpEventObjs.size() > 0)
			g_vpEventObjs[0]->SetIsActive(true);
		else if (g_pEnterObject && g_vpEventObjs.size() == 0)
		{
			g_pEnterObject->SetIsActive(true);
			g_pEnterObject->Update(true);
		}
	}

	for (int i = 0; i < g_vpEventObjs.size(); ++i)
	{
		if (g_vpEventObjs[i]) 
		{
			g_vpEventObjs[i]->Update(elapsedTime);

			if (g_vpEventObjs[i]->GetIntersect())
			{
				LoadMapData(g_pd3dDevice, g_pd3dCommandList, g_vpEventObjs[i]->GetFilePath(), true);
				g_vpEventObjs[i]->DestroyRunTime();
			}
				
		}
	}
}

void Scene::UpdatePlayerData(float elapsedTime)
{
	if (g_pPlayer)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->SetCameraPosition(m_pCamera->GetPosition3f());

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
			tmpPlayer->SetCameraRotation(XMFLOAT3(m_pCamera->GetPitch(), m_pCamera->GetYaw(), 0));
		}
	}
}

void Scene::UpdateFadeInOut(float elapsedTime)
{
	if (m_FadeState == 0)
	{
		m_FadeInValue -= elapsedTime / 2;
		if (m_FadeInValue <= 0.0f)
		{
			m_FadeState = 2;
			m_FadeInValue = 0.0f;
		}
	}
	else if (m_FadeState == 1)
	{
		m_FadeInValue += elapsedTime / 2;
		if (m_FadeInValue >= 1.0f)
		{
			m_FadeState = 2;
			m_FadeInValue = 1.0f;
		}
	}

	if (m_bFadeTimer)
	{
		m_ElapsedFadeTimer += elapsedTime;
		if (m_ElapsedFadeTimer >= m_FadeTimer)
		{
			m_bFadeTimer = false;
			m_ElapsedFadeTimer = 0.0f;
			m_FadeState = 1;
		}
	}
}

void Scene::UpdateSceneCamera(float elapsedTime)
{
	m_pCamera->Update(elapsedTime);
	if (m_vpCinematics.size() > m_nCurCinematicNum)
	{
		m_vpCinematics[m_nCurCinematicNum]->Update(elapsedTime);
		if (m_vpCinematics[m_nCurCinematicNum]->GetCinematicEnd())
			m_bInCinematic = false;

		m_pCinematicCamera->Update(elapsedTime);
	}
}

void Scene::UpdatePassCB(float totalTime, float elapsedTime)
{
	// �н� ���� : �� * ���� ��ȯ ��� ������Ʈ
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
	passConstant.AmbientLight = { 0.7f, 0.7f, 0.7f, 1.0f };
	passConstant.Lights[0].Direction = m_BaseLightDirections[m_nStageNum];
	passConstant.Lights[0].Strength = { 0.2f, 0.2f, 0.3f };
	//passConstant.Lights[0].Strength = { 0.0f, 0.0f, 0.0f };
	passConstant.Lights[0].Position = { 0, 30.0f, 20 };

	passConstant.FadeInValue = m_FadeInValue;

	m_pPassCB->CopyData(0, passConstant);
}

void Scene::UpdateShadowPassCB(float totalTime, float elapsedTime)
{
	float sceneBoundRadius = 400;
	if(m_nStageNum == 0)
		sceneBoundRadius = 600;

	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&m_BaseLightDirections[m_nStageNum]);
	lightDir = XMVector3Normalize(lightDir);
	XMFLOAT3 xmf3PlayerPos;
	if (m_bInCinematic)
		xmf3PlayerPos = m_pCinematicCamera->GetPosition3f();
	else
		xmf3PlayerPos = g_pPlayer->GetPosition();
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

	// �׸��� �н� ���� 
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
	if (m_FadeInValue <= 0)
		return;

	if (!m_bPaused)
	{
		ChangeShader(ShaderType::Shader_Skinned, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
		{
			if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
			{
				if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->GetIsAlive())
					continue;
				// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(0.0f);
				if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
					m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->Render(elapsedTime, pd3dCommandList);
			}
		}

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

		ChangeShader(ShaderType::Shader_BackFace, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Backface].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_Backface][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_Backface][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Backface][i]->Render(elapsedTime, pd3dCommandList);
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
			// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
			if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_TextureMesh, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Effect].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_Effect][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_Effect][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Effect][i]->Render(elapsedTime, pd3dCommandList);
		}
	}
	// img ������Ʈ
	{
		g_Shaders[ShaderType::Shader_Image]->ChangeShader(pd3dCommandList);
	
		//// Scene �׸��ڸ� �׽�Ʈ �� �ּ� ����
		//ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
		//pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);
		//
		//D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		//pd3dCommandList->SetGraphicsRootDescriptorTable(1, texHandle);

		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);

		m_pHP_Back->Render(elapsedTime, pd3dCommandList);
		m_pScore_Back->Render(elapsedTime, pd3dCommandList);

		m_pPlayerHP_Frame->Render(elapsedTime, pd3dCommandList);
		m_pPlayerHP_Bar->Render(elapsedTime, pd3dCommandList);
		m_pPlayer_Aim->Render(elapsedTime, pd3dCommandList);
		m_pEnemyName_Back->Render(elapsedTime, pd3dCommandList);

		m_pEnemyHP_Bar->Render(elapsedTime, pd3dCommandList);
		m_pEnemyHP_Frame->Render(elapsedTime, pd3dCommandList);
	}
}

void Scene::RenderSceneToShadowMap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_FadeInValue <= 0)
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
			// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
			//m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(0.0f);
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}

	// ���̴� ���� ����
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

	// �߸� ���� ���� ���� ��°�� ���̰� �׷����� ��
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
		// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
		//m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
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

	if (nShaderType == ShaderType::Shader_WireFrame)
		return;

	// Scene �׸��ڸ� 
	ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_nDescTableParameterIdx, texHandle);
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	if (m_FadeState != 2)
		return;
	// �ó׸�ƽ �����
	if (m_bInCinematic)
	{
		return;
	}
		
	if (m_pCamera)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		if (pKeybuffer[VK_RBUTTON] & 0xF0)
		{
			tmpCam->SetIsShoulderView(true);
		}
		else
		{
			tmpCam->SetIsShoulderView(false);
		}
		float dx, dy;
		dx = dy = 0;

		// ī�޶� �̵�
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
		g_pPlayer->ProcessInput(pKeybuffer);
	}
	
#if defined(_DEBUG)

#endif
}

void Scene::AnykeyProcess()
{
	// Ű�Է� ����
	if (m_bPressAnyKey && m_FadeState == 2)
	{
		// ���� ���۽�
		if (m_bGameStart)
		{
			m_vpCinematics[m_nCurCinematicNum]->Play();
			m_bPressAnyKey = false;
			m_bGameStart = false;
			m_bTitle = false;

			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title),
				Big_Text_UI_Idx_Title);
		}

		// ���� ������
		if (m_bGameOver)
		{
			m_bGameOver = false;
			m_bPressAnyKey = false;
			Restart();
			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Over),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Over),
				Big_Text_UI_Idx_Over);
		}
		// ���� �����
		if (m_bGameEnd)
		{
			m_bGameEnd = false;
			m_bThanks = false;
			m_bPressAnyKey = false;
			GameStart();
			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_End),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_End),
				Big_Text_UI_Idx_End);
		}
		return;
	}
}

void Scene::KeyDownEvent(WPARAM wParam)
{
#if defined(_DEBUG)
	switch (wParam)
	{
	case 'O':
		m_bGameEnd = true;
		break;
	case 'I':		// ���� ���������� ���� �׽�Ʈ�� 
		m_bNextStage = true;
		break;
	}

	// �ִϸ��̼� �׽�Ʈ �뵵
	for (int i = 0; i < g_vpCharacters.size(); ++i)
	{
		g_vpCharacters[i]->KeyDownEvent(wParam);
	}

#endif

	switch (wParam)
	{
	case 'P':
		m_bPaused = !m_bPaused;
		break;
	case VK_ESCAPE:
		if (m_bPaused)
			PostQuitMessage(0);
		m_bPaused = !m_bPaused;
		break;
	default:
		m_bPaused = false;
		break;
	}

	AnykeyProcess();

	// �ó׸�ƽ �����
	if (m_bInCinematic)
	{
		return;
	}
	
	if(g_pPlayer) g_pPlayer->KeyDownEvent(wParam);
}

void Scene::KeyUpEvent(WPARAM wParam)
{
	// �ó׸�ƽ �����
	if (m_bInCinematic)
		return;

	if (g_pPlayer) g_pPlayer->KeyUpEvent(wParam);
}

void Scene::LeftButtonDownEvent()
{
	m_bPaused = false;

	AnykeyProcess();

	// �ó׸�ƽ �����
	if (m_bInCinematic)
	{
		return;
	}

	if (g_pPlayer)
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->LeftButtonDownEvent();
	}
}

void Scene::RightButtonDownEvent()
{
	m_bPaused = false;

	AnykeyProcess();

	// �ó׸�ƽ �����
	if (m_bInCinematic)
	{
		return;
	}

	if (g_pPlayer) 
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->RightButtonDownEvent();
	}
}

void Scene::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName, bool bEvent)
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
			std::shared_ptr<Object> pObject;

			if(g_DefaultObjectData[pstrObjectName].objectType == Object_Monster)
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, XMFLOAT3(1,1,1), pstrObjectName, MONSTER_TRACK_CNT);
			else if (g_DefaultObjectData[pstrObjectName].objectType == Object_Event)
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, xmf3Scale, pstrObjectName, 0);
			else
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, XMFLOAT3(1, 1, 1), pstrObjectName, 0);

			if (bEvent)
				m_pEvnetObjects.emplace_back(pObject);
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
		objectData.xmf3Extents.x = nAnimationTracks;
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
		objectData.soundType = g_DefaultObjectData[pstrFileName].soundType;
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
			// �ε���� ���� ���� ���
			assert(false);
			//return nullptr;
		}
		else	// �̹� �ε��� ���� ���
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
		if (m_nStageNum == 1)
			objectData.xmf3Scale = XMFLOAT3(8, 8, 8);

		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		strcpy_s(pPlayer->m_pstrFileName, strFileName.c_str());
		wcscpy_s(pPlayer->m_pstrOutName, L"Player");
		pObject = std::static_pointer_cast<Object>(pPlayer);

		g_vpAllObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pPlayer);
		g_pPlayer = pPlayer;
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;

	case Object_Monster:
	{
		if (m_nStageNum == 1)
			objectData.xmf3Scale = XMFLOAT3(8, 8, 8);

		if (!strcmp(pstrFileName, ZOMBIE_MODEL_NAME))
		{
			std::shared_ptr<Zombie> pMonster = std::make_shared<Zombie>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"����");
		}
		else if (!strcmp(pstrFileName, HIGHZOMBIE_MODEL_NAME))
		{
			std::shared_ptr<HighZombie> pMonster = std::make_shared<HighZombie>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"��ȭ�� ����");
		}
		else if (!strcmp(pstrFileName, SCAVENGER_MODEL_NAME))
		{
			std::shared_ptr<Scavenger> pMonster = std::make_shared<Scavenger>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"�縶��");
		}
		else if (!strcmp(pstrFileName, GHOUL_MODEL_NAME))
		{
			std::shared_ptr<Ghoul> pMonster = std::make_shared<Ghoul>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"����");
		}
		else if (!strcmp(pstrFileName, CYBER_TWINS_MODEL_NAME))
		{
			std::shared_ptr<CyberTwins> pMonster = std::make_shared<CyberTwins>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"Ʈ����");
		}
		else if (!strcmp(pstrFileName, NECROMANCER_MODEL_NAME))
		{
			std::shared_ptr<Necromancer> pMonster = std::make_shared<Necromancer>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"���ɼ���");
		}
		else
		{
			// ��Ų �޽� �ִϸ��̼� �׽�Ʈ��
			std::shared_ptr<AnimTestCharacter> pMonster = std::make_shared<AnimTestCharacter>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
		}

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
	   
		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
	}
	break;

	case Object_Item:
	{
		std::shared_ptr<Item> pItem = std::make_shared<Item>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pItem);

		g_vpAllObjs.emplace_back(pObject);
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

		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
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

		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
	}
	break;

	case Object_Effect:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;
	case Object_Event:
	{
		std::shared_ptr<CEvent> pEvent = std::make_shared<CEvent>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		
		if (!strcmp(strFileName.c_str(), "Enter_Object"))
		{
			g_pEnterObject = pEvent;
			pEvent->SetVisible(true);
		}
		else
		{
			g_vpEventObjs.emplace_back(pEvent);
		}
		pObject = std::static_pointer_cast<Object>(pEvent);
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
	float cuttingPower = rand() % 30 + 40; 
	
	XMVECTOR noiseDir = XMLoadFloat3(&xmf3PlaneNormal);
	noiseDir *= -1 * (float)(rand() % 30) / 100;
	
	float directions[3] = {0,0,0};
	XMFLOAT3 xmf3PlaneNormals[3] = { XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(0,0,0) };
	UINT planeCnt = 0;

	XMVECTOR angularVel = XMLoadFloat3(&pObject->GetLookVector());
	angularVel *= direction;


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


		XMVECTOR cuttingVel = XMLoadFloat3(&xmf3PlaneNormal);
		cuttingVel += noiseDir;
		cuttingVel *= cuttingPower * direction * -1;
		XMFLOAT3 xmf3CuttingVel;
		XMStoreFloat3(&xmf3CuttingVel, cuttingVel);
		pCuttedObj->GetBody()->AddVelocity(xmf3CuttingVel);

		XMVECTOR inverseAngularVel = angularVel;
		inverseAngularVel *= -1;
		XMFLOAT3 xmf3InverseAngularVel;
		XMStoreFloat3(&xmf3InverseAngularVel, inverseAngularVel);
		pCuttedObj->GetBody()->AddAngleVelocity(xmf3InverseAngularVel);
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

	XMVECTOR cuttingVel = XMLoadFloat3(&xmf3PlaneNormal);
	cuttingVel += noiseDir;
	cuttingVel *= cuttingPower * direction;
	XMFLOAT3 xmf3CuttingVel;
	XMStoreFloat3(&xmf3CuttingVel, cuttingVel);
	tmpObject->GetBody()->AddVelocity(xmf3CuttingVel);

	XMFLOAT3 xmf3Angularvel;
	XMStoreFloat3(&xmf3Angularvel, angularVel);
	tmpObject->GetBody()->AddAngleVelocity(xmf3Angularvel);

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

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// ĳ���� �˻�
	for (int k = 0; k < g_vpCharacters.size(); ++k)
	{
		if (!g_vpCharacters[k]->GetIsAlive()) continue;
		ColliderBox* characterBox = (ColliderBox*)g_vpCharacters[k]->GetCollider().get();
		XMVECTOR position_1 = XMLoadFloat3(&g_vpCharacters[k]->GetPosition());

		if (!characterBox) continue;

		for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*characterBox, *g_ppColliderPlanes[i], m_CollisionData);
		}

		for (int i = 0; i < g_vpAllObjs.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (!g_vpAllObjs[i]->GetIsAlive()) continue;
			if (g_vpAllObjs[i]->GetColliderType() != ColliderType::Collider_Box) continue;

			if (!g_vpAllObjs[i]->GetCollider()->GetCharacterActive()) continue;

			ColliderBox* pColliderBox = (ColliderBox*)g_vpAllObjs[i]->GetCollider().get();

			if (characterBox == pColliderBox) continue;

			// �ڽ� �˻� ���� ���� ���� ���� �˻� 
			//if (!characterBox->GetBoundingSphere()->Intersects(*(pColliderBox->GetBoundingSphere().get())))
			//	continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpAllObjs[i]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			Object* pObject1 = g_vpCharacters[k].get();
			Object* pObject2 = g_vpAllObjs[i].get();
			CollisionDetector::BoxAndBox(*characterBox, *pColliderBox, m_CollisionData, pObject1, pObject2);
		}
	}

	m_CollisionData.friction = 0.9f;
	// ������ �˻�
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		XMVECTOR planeNormal = XMLoadFloat3(&g_ppColliderPlanes[i]->GetDirection());
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox) continue;
			//if (!colliderBox->GetBoundingSphere().Intersects(planeNormal))
			//	continue;

			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// �ڽ����� �˻�
	for (int i = 0; i < g_vpWorldObjs.size(); ++i)
	{
		if (!g_vpWorldObjs[i]->GetIsAlive()) continue;
		if (g_vpWorldObjs[i]->GetColliderType() != ColliderType::Collider_Box) continue;
		if (g_vpWorldObjs[i]->GetObjectType() != ObjectType::Object_Movable) continue;
		ColliderBox* pColliderBox1 = (ColliderBox*)g_vpWorldObjs[i]->GetCollider().get();

		XMVECTOR position_1 = XMLoadFloat3(&g_vpWorldObjs[i]->GetPosition());

		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (g_vpWorldObjs[i] == g_vpWorldObjs[k]) continue;
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box) continue;

			ColliderBox* pColliderBox2 = (ColliderBox*)g_vpWorldObjs[k]->GetCollider().get();
			if (pColliderBox1 == pColliderBox2) continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpWorldObjs[k]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			// �ڽ� �˻� ���� ���� ���� ���� �˻� 
			//if (!pColliderBox1->GetBoundingSphere()->Intersects(*(pColliderBox2->GetBoundingSphere().get())))
			//	continue;

			Object* pObject1 = g_vpWorldObjs[i].get();
			Object* pObject2 = g_vpWorldObjs[k].get();
			CollisionDetector::BoxAndBox(*pColliderBox1, *pColliderBox2, m_CollisionData, pObject1, pObject2);
		}
	}

	// �߸� ��ü �˻�
	// ������ �˻�
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
	// �ڽ��� �˻�
	for (int i = 0; i < g_vpCuttedObjects.size(); ++i)
	{
		if (!g_vpCuttedObjects[i]->GetIsAlive()) continue;
		ColliderBox* pColliderBox1 = (ColliderBox*)g_vpCuttedObjects[i]->GetCollider().get();

		XMVECTOR position_1 = XMLoadFloat3(&g_vpCuttedObjects[i]->GetPosition());

		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box) continue;

			ColliderBox* pColliderBox2 = (ColliderBox*)g_vpWorldObjs[k]->GetCollider().get();
			if (pColliderBox1 == pColliderBox2) continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpWorldObjs[k]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			Object* pObject1 = g_vpCuttedObjects[i].get();
			Object* pObject2 = g_vpWorldObjs[k].get();
			CollisionDetector::BoxAndBox(*pColliderBox1, *pColliderBox2, m_CollisionData, pObject1, pObject2);
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

	// ��ü ��ȸ
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

	for (int i = 0; i < g_vpEventObjs.size(); ++i)
	{
		if (!g_vpEventObjs[i]->GetIsAlive())
		{
			g_vpEventObjs[i]->DestroyRunTime();
			g_vpEventObjs.erase(g_vpEventObjs.begin() + i);
		}
	}


	for (int i = 0; i < m_pEvnetObjects.size(); ++i)
	{
		if (!m_pEvnetObjects[i]->GetIsAlive())
		{
			m_pEvnetObjects[i]->DestroyRunTime();
			m_pEvnetObjects.erase(m_pEvnetObjects.begin() + i);
		}
	}

	// ���̾� ��ȸ
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

// Sound
void Scene::EmitSound(const char* pstrFilePath, bool bLoop)
{
	std::shared_ptr<CSound> newSound = std::make_shared<CSound>(pstrFilePath, bLoop);
	newSound->Play();
	m_vpSounds.emplace_back(newSound);
}

void Scene::EmitSound(const char* pstrFilePath, bool bLoop, float pitch, float volume)
{
	std::shared_ptr<CSound> newSound = std::make_shared<CSound>(pstrFilePath, bLoop);
	newSound->Play();
	newSound->SetPitch(pitch);
	newSound->SetVolme(volume);
	m_vpSounds.emplace_back(newSound);
}

void Scene::EmitHitSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_HIT_SOUND);
		tmpRand = rand() % 3 + 1;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_HIT_SOUND);
		tmpRand = rand() % 1 + 1;
		break;
	case Sound_Character:
		strcpy_s(pstrFilePath, CHARACTER_HIT_SOUND);
		tmpRand = rand() % 3 + 1;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");
	EmitSound(pstrFilePath, bLoop, 1.0f, 0.3f);
}

void Scene::EmitCutSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;
	float volume = 0.3f;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_CUT_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_CUT_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_CUT_SOUND);
		tmpRand = rand() % 1 + 1;
		break;
	case Sound_Character:
		strcpy_s(pstrFilePath, CHARACTER_CUT_SOUND);
		tmpRand = rand() % 8 + 1;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");

	EmitSound(pstrFilePath, bLoop, 1.0f, volume);
}


void Scene::EmitBrokenSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;
	float volume = 0.3f;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_BROKEN_SOUND);
		tmpRand = rand() % 4 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_BROKEN_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		volume = 0.1f;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_BROKEN_SOUND);
		tmpRand = rand() % 1 + 1;
		volume = 0.5f;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");
	EmitSound(pstrFilePath, bLoop, 1.0f, volume);
}

void Scene::ChangeStage()
{
	if (m_bNextStage)
	{
		m_pTextUIs->UpdateTextUI(L"Loading...", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Loading), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Loading), Text_UI_Idx_Loading);
		if (m_FadeInValue <= 0.0f)
		{
			m_PlayerStartScore = ((Player*)g_pPlayer.get())->GetScore();

			m_nStageNum += 1;
			m_nStageNum %= 3;
			StageStart(m_nStageNum);
			m_bFadeTimer = true;
			m_bNextStage = false;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
		}
	}
}

void Scene::GameStart()
{
	// ���̵� �ƿ� ���·� ����
	m_FadeState = 0;
	m_FadeInValue = 0.0f;

	if (g_pPlayer)
	{
		g_pPlayer->DestroyRunTime();
		g_pPlayer = nullptr;
		m_PlayerStartScore = 0;
	}

	// 0�� ������������ ����
	m_nStageNum = 0;
	// �������� �ʱ�ȭ �Լ�
	StageStart(m_nStageNum);

	// ���̵� ��
	m_FadeState = 1;

	// ���� �ó׸�ƽ �ʱ�ȭ
	InitCinematic();

	// �÷��� ���� ���� => Ű�Է� �ĺ��� ����
	PlayCinematic(m_nCurCinematicNum);
	m_vpCinematics[m_nCurCinematicNum]->Stop();
	// Ű�Է� ����
	m_bPressAnyKey = true;
	m_bGameStart = true;
	m_bTitle = true;
}

void Scene::Restart()
{
	// �������� ���� ������ ����
	((Player*)(g_pPlayer.get()))->SetScore(m_PlayerStartScore);
	((Player*)(g_pPlayer.get()))->SetHP(g_pPlayer->GetMaxHP());

	m_nStageNum -= 1;
	m_bNextStage = true;
}

void Scene::GameOver()
{
	if (g_pPlayer->GetHP() <= 0)
		m_bGameOver = true;

	if (m_bGameOver)
	{
		if (m_FadeInValue <= 0.0f)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"Game Over",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Over),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Over),
				Big_Text_UI_Idx_Over);

			m_bPressAnyKey = true;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
		}
	}
}

void Scene::GameEnd()
{
	if (m_bGameEnd)
	{
		if (m_FadeInValue <= 0.0f)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"The End.",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_End),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_End),
				Big_Text_UI_Idx_End);

			m_bThanks = true;
			m_bPressAnyKey = true;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
		}
	}
}

void Scene::StageStart(UINT nMapNum)
{
	float hp = 300;
	UINT score = 0;
	if (g_pPlayer)
	{
		hp = g_pPlayer->GetHP();
		score = ((Player*)(g_pPlayer.get()))->GetScore();
	}

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		g_vpAllObjs[i]->DestroyRunTime();
	}

	ClearObjectLayer();

	// �÷��̾�
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 10, -20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f),
		CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);
	g_pPlayer->SetHP(hp);
	((Player*)(g_pPlayer.get()))->SetScore(score);

	// ī�޶� �ʱ�ȭ
	m_pCamera = nullptr;
	m_pCamera = std::make_unique<Third_Person_Camera>(g_pPlayer);
	m_pCamera->SetPitch(15);

	// ����
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);
	// �ٴ�
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);

	switch (nMapNum)
	{
	case 0:
		g_pPlayer->SetRotate(XMFLOAT3(0, 90, 0));
		m_pCamera->SetYaw(90);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, -1, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -460);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -150);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/OutSideMap", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/OutSideMonsterData", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/Outside_Event", false);
		//LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/Outside_Cine_1");
		break;
	case 1:
		g_pPlayer->SetRotate(XMFLOAT3(0, 180, 0));
		m_pCamera->RotateY(180);
		g_pPlayer->GetBody()->SetPosition(XMFLOAT3(0, 10, 10));
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, -1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -40);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Hospital/HospitalInsideMap", false);
		break;
	case 2:
		g_pPlayer->SetRotate(XMFLOAT3(0, 90, 0));
		m_pCamera->RotateY(90);
		g_pPlayer->GetBody()->SetPosition(XMFLOAT3(20, 10, 0));
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, -1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -80);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Dungeon/DungeonMap", false);
		break;

	default:
		assert(false);
	}
}

bool Scene::InitCinematic()
{
	for (int i = 0; i < m_vpCinematics.size(); ++i)
	{
		m_vpCinematics[i]->Destroy();
		m_vpCinematics.erase(m_vpCinematics.begin() + i);
	}
	
	m_pCinematicCamera = std::make_shared<CinematicCamera>();
	m_pCinematicCamera->SetPosition(XMFLOAT3(35, 20, -55));
	m_pCinematicCamera->SetOrientationByRotation(XMFLOAT3(10, -30, 0));
	m_pCinematicCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	//std::shared_ptr<Object> pObject;
	//XMFLOAT3 position;
	//XMFLOAT3 rotation;
	//XMFLOAT3 scale;
	//auto pFunction = nullptr;

	// ī�޶� ���� �� ����
	pCinematic->AddCamera(m_pCinematicCamera, XMFLOAT3(35, 20, -55), XMFLOAT3(10, -30, 0));
	pCinematic->AddCameraKeyFrame(1.0f, XMFLOAT3(35, 20, -55), XMFLOAT3(10, -30, 0));
	pCinematic->AddCameraKeyFrame(5.0f, XMFLOAT3(-50, 38, -20), XMFLOAT3(15, 90, 0));
	pCinematic->AddCameraKeyFrame(6.0f, XMFLOAT3(-48, 38, -20), XMFLOAT3(15, 90, 0));
	//pCinematic->AddCameraKeyFrame(1.0f, XMFLOAT3(0, 0, 0),   XMFLOAT3(0, 0, 0));
	//pCinematic->AddCameraKeyFrame(1.5f, XMFLOAT3(0, 50, 0),  XMFLOAT3(0, 90, 0));

	// ������Ʈ ���� �� ����
	//pObject = m_vObjectLayer[RenderLayer::Render_Skinned][1];
	//position = pObject->GetPosition();
	//rotation = pObject->GetRotation();
	//scale = pObject->GetScale();
	//
	//pCinematic->AddTrack(pObject, position, rotation, scale);
	//pCinematic->AddKeyFrame(0, 0.5f, position, XMFLOAT3(0, 30, 0), scale, pFunction);
	//pCinematic->AddKeyFrame(0, 1.0f, position, XMFLOAT3(0, 30, 0), scale, pFunction);
	//pCinematic->AddKeyFrame(0, 1.5f, position, XMFLOAT3(0, 90, 0), scale, pFunction);
	//
	//pObject = m_vObjectLayer[RenderLayer::Render_TextureMesh][5];
	//position = pObject->GetPosition();
	//rotation = pObject->GetRotation();
	//scale = pObject->GetScale();
	//
	//pCinematic->AddTrack(pObject, position, rotation, scale);
	//pCinematic->AddKeyFrame(1, 0.5f, position, XMFLOAT3(0, 30, 0), scale, pFunction);
	//pCinematic->AddKeyFrame(1, 1.0f, position, XMFLOAT3(0, 30, 100), scale, pFunction);
	//pCinematic->AddKeyFrame(1, 1.5f, position, XMFLOAT3(0, 90, 500), scale, pFunction);

	m_vpCinematics.emplace_back(pCinematic);

	return true;
}

bool Scene::InitEvent(UINT nMapNum)
{
	// ���� �̺�Ʈ ��� ����
	for (int i = 0; i < g_vpEventObjs.size(); ++i)
	{
		g_vpEventObjs[i]->Destroy();
	}

	switch (nMapNum)
	{
	default:
		break;
	}

	return true;
}